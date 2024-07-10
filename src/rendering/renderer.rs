use std::cell::RefCell;

use nalgebra::Vector2;
use raylib::{
    color::Color, drawing::RaylibDraw, ffi::KeyboardKey, math::Rectangle, RaylibHandle,
    RaylibThread,
};

use crate::ui::MouseEventHandler;
use crate::APP_STATE;
use crate::{registry::Registry, ui::UiId};

use super::{
    area::{Area, AreaId, AreaType},
    boundary::{Boundary, BoundaryId, BoundaryOrientation},
};

pub fn to_raylib(x: Vector2<f64>) -> raylib::math::Vector2 {
    raylib::math::Vector2 {
        x: x.x as f32,
        y: x.y as f32,
    }
}

pub fn to_nalgebra(x: raylib::math::Vector2) -> Vector2<f64> {
    Vector2::<f64>::new(x.x as f64, x.y as f64)
}

thread_local!(pub static AREA_MAP: RefCell<Registry<AreaId, Area>> = RefCell::new(Registry::new()));
thread_local!(pub static BDRY_MAP: RefCell<Registry<BoundaryId, Boundary>> = RefCell::new(Registry::new()));

#[derive(Debug)]
pub struct Renderer {
    screen_w: i32,
    screen_h: i32,
    mouse_bdry_tol: f64,
    mouse_pos: Vector2<f64>,
    grabbed: Option<BoundaryId>,
    next_area_id: AreaId,
    next_bdry_id: BoundaryId,
}

impl Renderer {
    pub fn new(screen_w: i32, screen_h: i32, rl: &mut RaylibHandle, t: &RaylibThread) -> Self {
        AREA_MAP.with_borrow_mut(|areas| {
            areas.insert(Area::new(
                AreaType::Empty,
                areas.next_id(),
                Rectangle {
                    x: 0.0,
                    y: 0.0,
                    width: screen_w as f32,
                    height: screen_h as f32,
                },
                Vector2::default(),
                rl.load_render_texture(t, screen_w as u32, screen_h as u32)
                    .unwrap(),
                rl,
            ));
        });

        Self {
            screen_w,
            screen_h,
            mouse_bdry_tol: 5.0,
            mouse_pos: Vector2::default(),
            grabbed: None,
            next_area_id: AreaId(1),
            next_bdry_id: BoundaryId(0),
        }
    }

    pub fn draw(&mut self, rl: &mut RaylibHandle, t: &RaylibThread) {
        let mut d = rl.begin_drawing(t);
        d.clear_background(Color::BLACK);

        AREA_MAP.with_borrow_mut(|areas| {
            for area in areas.values_mut() {
                area.draw(&mut d, t);
            }
        });

        BDRY_MAP.with_borrow(|bdrys| {
            for bdry in bdrys.values() {
                bdry.draw(&mut d, t);
            }
        });

        d.draw_fps(self.screen_w - 100, 900);
    }

    pub fn update(&mut self, rl: &mut RaylibHandle) {
        let mut state = APP_STATE.lock().unwrap();
        if rl.is_key_pressed(KeyboardKey::KEY_Q) {
            state.running = false;
        }

        let mouse_pos = to_nalgebra(rl.get_mouse_position());
        self.receive_mouse_pos(mouse_pos);
    }

    pub fn split_area(
        &mut self,
        mouse_pos: Vector2<f64>,
        orientation: BoundaryOrientation,
        rl: &mut RaylibHandle,
        t: &RaylibThread,
    ) {
        AREA_MAP.with_borrow_mut(|area_map| {
            let next_area_id = area_map.next_id();
            if let Some(to_split) =
                area_map.get_mut(&self.find_area(mouse_pos, area_map).unwrap_or_default())
            {
                let new_rect = match orientation {
                    BoundaryOrientation::Vertical => Rectangle {
                        x: 0.0,
                        y: 0.0,
                        width: to_split.screen_rect.width / 2.0,
                        height: to_split.screen_rect.height,
                    },
                    BoundaryOrientation::Horizontal => Rectangle {
                        x: 0.0,
                        y: 0.0,
                        width: to_split.screen_rect.width,
                        height: to_split.screen_rect.height / 2.0,
                    },
                };
                let new_pos = match orientation {
                    BoundaryOrientation::Vertical => Vector2::<f64>::new(
                        to_split.screen_pos.x + to_split.screen_rect.width as f64 / 2.0,
                        to_split.screen_pos.y,
                    ),
                    BoundaryOrientation::Horizontal => Vector2::<f64>::new(
                        to_split.screen_pos.x,
                        to_split.screen_pos.y + to_split.screen_rect.height as f64 / 2.0,
                    ),
                };
                let new_area = Area::new(
                    AreaType::Empty,
                    next_area_id,
                    new_rect,
                    new_pos,
                    rl.load_render_texture(t, self.screen_w as u32, self.screen_h as u32)
                        .unwrap(),
                    rl,
                );
                match orientation {
                    BoundaryOrientation::Horizontal => {
                        to_split.screen_rect.height /= 2.0;
                    }
                    BoundaryOrientation::Vertical => {
                        to_split.screen_rect.width /= 2.0;
                    }
                }
                to_split.build(rl);

                let mut bdry = Boundary::new(self.next_bdry_id, orientation);
                bdry.side1.push(to_split.id);
                bdry.side2.push(new_area.id);

                BDRY_MAP.with_borrow_mut(|bdry_map| {
                    for id in to_split.further_down_bdry_tree(bdry_map) {
                        if let Some(existing_bdry) = bdry_map.get_mut(&id) {
                            if existing_bdry.orientation == bdry.orientation {
                                existing_bdry.side1.retain(|id| *id != to_split.id);
                            }
                            if !existing_bdry.side1.contains(&new_area.id) {
                                existing_bdry.side1.push(new_area.id);
                            }
                        }
                    }
                    for id in to_split.further_up_bdry_tree(bdry_map) {
                        if let Some(existing_bdry) = bdry_map.get_mut(&id) {
                            if existing_bdry.orientation != bdry.orientation {
                                if !existing_bdry.side2.contains(&new_area.id) {
                                    existing_bdry.side2.push(new_area.id);
                                }
                            }
                        }
                    }
                    bdry_map.insert(bdry);
                });
                area_map.insert(new_area);
            }
        });
    }

    pub fn collapse_boundary(&mut self, mouse_pos: Vector2<f64>, rl: &mut RaylibHandle) {
        if let Some(hovered) = self.find_boundary(mouse_pos, self.mouse_bdry_tol) {
            BDRY_MAP.with_borrow_mut(|bdry_map| {
                if bdry_map[hovered].can_collapse() {
                    let mut bdry = bdry_map.remove(&hovered).unwrap();
                    if bdry.side2.len() != 1 {
                        panic!("Cannot delete boundary with more than one child area");
                    }
                    bdry.collapse(bdry_map, rl);
                    AREA_MAP.with_borrow_mut(|area_map| area_map.remove(&bdry.side2[0]));
                }
            });
        }
    }

    fn find_boundary(&self, mouse_pos: Vector2<f64>, radius: f64) -> Option<BoundaryId> {
        let mut out = None;
        let mut closest_dist = f64::INFINITY;
        BDRY_MAP.with_borrow(|bdry_map| {
            for (id, bdry) in bdry_map.iter() {
                let dist = bdry.distance_to_point(mouse_pos);
                if dist < closest_dist {
                    out = Some(*id);
                    closest_dist = dist;
                }
            }
        });
        if closest_dist < radius {
            out
        } else {
            None
        }
    }

    fn find_area(
        &self,
        mouse_pos: Vector2<f64>,
        area_map: &Registry<AreaId, Area>,
    ) -> Option<AreaId> {
        let mut out = None;
        for (id, area) in area_map.iter() {
            if area.contains_point(mouse_pos) {
                out = Some(*id);
            }
        }
        out
    }

    pub fn dump_layout(&self) {
        println!("Areas");
        AREA_MAP.with_borrow(|area_map| {
            for area in area_map.values() {
                println!("  {:#?}", area);
            }
        });
        println!("Boundaries");
        BDRY_MAP.with_borrow(|bdry_map| {
            for bdry in bdry_map.values() {
                println!("  {:#?}", bdry);
            }
        });
    }
}

impl MouseEventHandler for Renderer {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        true
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        self.mouse_pos = mouse_pos;
        AREA_MAP.with_borrow_mut(|area_map| {
            for area in area_map.values_mut() {
                area.receive_mouse_pos(mouse_pos);
            }
        });

        let maybe_hovered = self.find_boundary(mouse_pos, self.mouse_bdry_tol);
        BDRY_MAP.with_borrow_mut(|bdry_map| {
            for (id, bdry) in bdry_map.iter_mut() {
                bdry.hovered = false;
                if let Some(hovered) = maybe_hovered {
                    if hovered == *id {
                        bdry.hovered = true;
                    }
                }
            }
        });
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>) {
        match self.grabbed {
            Some(_) => {}
            None => match self.find_boundary(mouse_pos, self.mouse_bdry_tol) {
                Some(hovered) => {
                    self.grabbed = Some(hovered);
                }
                None => {
                    AREA_MAP.with_borrow_mut(|area_map| {
                        for area in area_map.values_mut() {
                            area.receive_mouse_down(mouse_pos);
                        }
                    });
                }
            },
        }
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>) {
        match self.grabbed {
            Some(_) => self.grabbed = None,
            None => {
                AREA_MAP.with_borrow_mut(|area_map| {
                    for area in area_map.values_mut() {
                        area.receive_mouse_up(mouse_pos);
                    }
                });
            }
        }
    }

    fn receive_mouse_wheel(&mut self, mouse_pos: Vector2<f64>, movement: f64) {
        AREA_MAP.with_borrow_mut(|area_map| {
            for area in area_map.values_mut() {
                area.receive_mouse_wheel(mouse_pos, movement);
            }
        });
    }

    fn get_on_mouse_enter(&mut self) -> Option<&mut Box<(dyn FnMut(UiId) + 'static)>> {
        None
    }

    fn set_on_mouse_enter(&mut self, f: Box<(dyn FnMut(UiId) + 'static)>) {}
}

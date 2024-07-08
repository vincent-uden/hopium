use std::cell::RefCell;
use std::collections::HashMap;

use nalgebra::Vector2;
use raylib::{
    color::Color, drawing::RaylibDraw, ffi::KeyboardKey, math::Rectangle, RaylibHandle,
    RaylibThread,
};

use crate::app_state;

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

pub trait MouseEventHandler {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool;

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>);

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>);

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>);

    fn receive_mouse_wheel(&mut self, mouse_pos: Vector2<f64>, movement: f64);

    // TODO: Set mouse event listeners
}

thread_local!(pub static AREA_MAP: RefCell<HashMap<AreaId, Area>> = RefCell::new(HashMap::new()));
thread_local!(pub static BDRY_MAP: RefCell<HashMap<BoundaryId, Boundary>> = RefCell::new(HashMap::new()));

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
        AREA_MAP.with(|areas| {
            areas.borrow_mut().insert(
                AreaId(0),
                Area::new(
                    AreaType::Empty,
                    AreaId(0),
                    Rectangle {
                        x: 0.0,
                        y: 0.0,
                        width: screen_w as f32,
                        height: screen_h as f32,
                    },
                    Vector2::default(),
                    rl.load_render_texture(t, screen_w as u32, screen_h as u32)
                        .unwrap(),
                ),
            );
        });
        let out = Self {
            screen_w,
            screen_h,
            mouse_bdry_tol: 5.0,
            mouse_pos: Vector2::default(),
            grabbed: None,
            next_area_id: AreaId(1),
            next_bdry_id: BoundaryId(0),
        };
        out
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
        let mut state = app_state.lock().unwrap();
        if rl.is_key_pressed(KeyboardKey::KEY_Q) {
            state.running = false;
        }
    }

    pub fn split_area(
        &mut self,
        mouse_pos: Vector2<f64>,
        orientation: BoundaryOrientation,
        rl: &mut RaylibHandle,
        t: &RaylibThread,
    ) {
        AREA_MAP.with_borrow_mut(|area_map| {
            if let Some(to_split) =
                area_map.get_mut(&self.find_area(mouse_pos, &area_map).unwrap_or_default())
            {
                let new_rect = match orientation {
                    BoundaryOrientation::Horizontal => Rectangle {
                        x: 0.0,
                        y: 0.0,
                        width: to_split.screen_rect.width / 2.0,
                        height: to_split.screen_rect.height,
                    },
                    BoundaryOrientation::Vertical => Rectangle {
                        x: 0.0,
                        y: 0.0,
                        width: to_split.screen_rect.width,
                        height: to_split.screen_rect.height / 2.0,
                    },
                };
                let new_pos = match orientation {
                    BoundaryOrientation::Horizontal => Vector2::<f64>::new(
                        to_split.screen_pos.x + to_split.screen_rect.width as f64 / 2.0,
                        to_split.screen_pos.y,
                    ),
                    BoundaryOrientation::Vertical => Vector2::<f64>::new(
                        to_split.screen_pos.x,
                        to_split.screen_pos.y + to_split.screen_rect.height as f64 / 2.0,
                    ),
                };
                let new_area = Area::new(
                    AreaType::Empty,
                    self.next_area_id,
                    new_rect,
                    new_pos,
                    rl.load_render_texture(t, self.screen_w as u32, self.screen_h as u32)
                        .unwrap(),
                );
                self.next_area_id = self.next_area_id.increment();
                to_split.screen_rect.width /= 2.0;

                let mut bdry = Boundary::new(self.next_bdry_id, orientation);
                self.next_bdry_id = self.next_bdry_id.increment();
                bdry.side1.push(to_split.id);
                bdry.side2.push(new_area.id);

                BDRY_MAP.with_borrow_mut(|bdry_map| {
                    if let Some(existing_bdry) = bdry_map.get_mut(
                        &to_split
                            .further_down_bdry_tree(&bdry_map)
                            .unwrap_or_default(),
                    ) {
                        existing_bdry.side1.retain(|id| *id != to_split.id);
                        existing_bdry.side1.push(new_area.id);
                    }
                    bdry_map.insert(bdry.id, bdry);
                });
                area_map.insert(new_area.id, new_area);
            }
        });
    }

    pub fn collapse_boundary(&mut self, mouse_pos: Vector2<f64>) {
        match self.find_boundary(mouse_pos, self.mouse_bdry_tol) {
            Some(hovered) => {
                BDRY_MAP.with_borrow_mut(|bdry_map| {
                    let mut bdry = bdry_map.remove(&hovered).unwrap();
                    if bdry.side2.len() != 1 {
                        panic!("Cannot delete boundary with more than one child area");
                    }
                    bdry.collapse();
                    AREA_MAP.with_borrow_mut(|area_map| area_map.remove(&bdry.side2[0]));
                });
            }
            None => {}
        }
    }

    fn find_boundary(&self, mouse_pos: Vector2<f64>, radius: f64) -> Option<BoundaryId> {
        let mut out = None;
        let mut closest_dist = f64::INFINITY;
        BDRY_MAP.with_borrow(|bdry_map| {
            for (id, bdry) in bdry_map.iter() {
                let dist = bdry.distance_to_point(mouse_pos);
                if dist < closest_dist {
                    out = Some(id.clone());
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
        area_map: &HashMap<AreaId, Area>,
    ) -> Option<AreaId> {
        let mut out = None;
        for (id, area) in area_map.iter() {
            if area.contains_point(mouse_pos) {
                out = Some(id.clone());
            }
        }
        out
    }
}

impl MouseEventHandler for Renderer {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        todo!()
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    fn receive_mouse_wheel(&mut self, mouse_pos: Vector2<f64>, movement: f64) {
        todo!()
    }
}

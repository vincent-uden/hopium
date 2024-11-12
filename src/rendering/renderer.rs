use std::cell::RefCell;

use nalgebra::Vector2;
use raylib::ffi::TextureFilter;
use raylib::texture::RaylibTexture2D;
use raylib::{
    color::Color, drawing::RaylibDraw, ffi::KeyboardKey, math::Rectangle, RaylibHandle,
    RaylibThread,
};

use crate::combined_draw_handle::CombinedDrawHandle;
use crate::event::Event;
use crate::modes::{KeyMods, MousePress};
use crate::ui::command_palette::CommandPalette;
use crate::ui::data_entry::DataEntry;
use crate::ui::{Drawable, MouseEventHandler};
use crate::{registry::Registry, ui::UiId};
use crate::{APP_STATE, EVENT_QUEUE, MODE_STACK};

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

pub struct Renderer {
    screen_w: i32,
    screen_h: i32,
    mouse_bdry_tol: f64,
    mouse_pos: Vector2<f64>,
    grabbed: Option<BoundaryId>,
    grabbed_at: Vector2<f64>,
    next_area_id: AreaId,
    next_bdry_id: BoundaryId,
    command_palette: CommandPalette,
    data_entry: DataEntry,
}

impl Renderer {
    pub fn new(screen_w: i32, screen_h: i32, rl: &mut RaylibHandle, t: &RaylibThread) -> Self {
        let texture = rl
            .load_render_texture(t, screen_w as u32, screen_h as u32)
            .unwrap();
        texture.set_texture_filter(t, TextureFilter::TEXTURE_FILTER_TRILINEAR);
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
                texture,
                rl,
            ));
        });

        Self {
            screen_w,
            screen_h,
            mouse_bdry_tol: 5.0,
            mouse_pos: Vector2::default(),
            grabbed: None,
            grabbed_at: Vector2::default(),
            next_area_id: AreaId(1),
            next_bdry_id: BoundaryId(0),
            command_palette: CommandPalette::new(Vector2::new(screen_w as f64, screen_h as f64)),
            data_entry: DataEntry::new(Vector2::new(screen_w as f64, screen_h as f64)),
        }
    }

    pub fn draw(&mut self, rl: &mut RaylibHandle, t: &RaylibThread) {
        let mut d = rl.begin_drawing(t);
        d.clear_background(Color::BLACK);

        let mut combined_draw_handle = CombinedDrawHandle::RaylibDrawHandle(d);

        AREA_MAP.with_borrow_mut(|areas| {
            for area in areas.values_mut() {
                area.draw(&mut combined_draw_handle, t);
            }
        });

        BDRY_MAP.with_borrow(|bdrys| {
            for bdry in bdrys.values() {
                bdry.draw(&mut combined_draw_handle, t);
            }
        });

        combined_draw_handle.draw_fps(self.screen_w - 100, 900);
        let ms = MODE_STACK.lock().unwrap();
        combined_draw_handle.draw_text(
            &format!("{:?}", &ms.innermost_mode()),
            10,
            880,
            20,
            Color::WHITE,
        );

        {
            let state = APP_STATE.lock().unwrap();
            if state.command_palette_open {
                self.command_palette.draw(&mut combined_draw_handle, t);
            }
            if let Some(form) = &state.form {
                self.data_entry.draw(&mut combined_draw_handle, t);
            }
        }
    }

    pub fn update(&mut self, rl: &mut RaylibHandle) {
        self.command_palette.update(rl);
        self.data_entry.update(rl);

        let mouse_pos = to_nalgebra(rl.get_mouse_position());
        self.receive_mouse_pos(mouse_pos);

        let ms = MODE_STACK.lock().unwrap();
        let shift = rl.is_key_down(KeyboardKey::KEY_LEFT_SHIFT)
            || rl.is_key_down(KeyboardKey::KEY_RIGHT_SHIFT);
        let ctrl = rl.is_key_down(KeyboardKey::KEY_LEFT_CONTROL)
            || rl.is_key_down(KeyboardKey::KEY_RIGHT_CONTROL);
        let l_alt = rl.is_key_down(KeyboardKey::KEY_LEFT_ALT);
        let r_alt = rl.is_key_down(KeyboardKey::KEY_RIGHT_ALT);
        for button in &ms.all_mouse_buttons {
            if rl.is_mouse_button_pressed(*button) {
                let press = MousePress {
                    button: *button,
                    shift,
                    ctrl,
                    l_alt,
                    r_alt,
                };
                self.receive_mouse_down(mouse_pos, &press);
            }
            if rl.is_mouse_button_released(*button) {
                let press = MousePress {
                    button: *button,
                    shift,
                    ctrl,
                    l_alt,
                    r_alt,
                };
                self.receive_mouse_up(mouse_pos, &press);
            }
        }
        let mods = KeyMods {
            shift,
            ctrl,
            l_alt,
            r_alt,
        };
        self.receive_mouse_wheel(mouse_pos, rl.get_mouse_wheel_move().into(), &mods);

        AREA_MAP.with_borrow_mut(|area_map| {
            for area in area_map.values_mut() {
                area.update(rl);
            }
        });
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
                let texture = rl
                    .load_render_texture(t, self.screen_w as u32, self.screen_h as u32)
                    .unwrap();
                texture.set_texture_filter(t, TextureFilter::TEXTURE_FILTER_TRILINEAR);
                let new_area = Area::new(
                    AreaType::Empty,
                    next_area_id,
                    new_rect,
                    new_pos,
                    texture,
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
                            if existing_bdry.orientation != bdry.orientation
                                && !existing_bdry.side2.contains(&new_area.id)
                            {
                                existing_bdry.side2.push(new_area.id);
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

    pub fn move_boundary(&mut self, end_pos: Vector2<f64>, bdry_id: BoundaryId) {
        BDRY_MAP.with_borrow_mut(|bdry_map| bdry_map[bdry_id].move_boundary(end_pos));
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

            if let Some(bdry_id) = self.grabbed {
                bdry_map[bdry_id].move_boundary(mouse_pos);
            }
        });
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &MousePress) {
        match self.grabbed {
            Some(_) => {}
            None => match self.find_boundary(mouse_pos, self.mouse_bdry_tol) {
                Some(hovered) => {
                    self.grabbed = Some(hovered);
                    self.grabbed_at = mouse_pos;
                }
                None => {
                    AREA_MAP.with_borrow_mut(|area_map| {
                        for area in area_map.values_mut() {
                            area.receive_mouse_down(mouse_pos, press);
                        }
                    });
                }
            },
        }
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &MousePress) {
        match self.grabbed {
            Some(id) => {
                let mut eq = EVENT_QUEUE.lock().unwrap();
                eq.post_event(Event::BoundaryMoved {
                    start_pos: self.grabbed_at,
                    end_pos: mouse_pos,
                    bdry_id: id,
                });
                self.grabbed = None;
            }
            None => {
                AREA_MAP.with_borrow_mut(|area_map| {
                    for area in area_map.values_mut() {
                        area.receive_mouse_up(mouse_pos, press);
                    }
                });
            }
        }
    }

    fn receive_mouse_wheel(&mut self, mouse_pos: Vector2<f64>, movement: f64, mods: &KeyMods) {
        AREA_MAP.with_borrow_mut(|area_map| {
            for area in area_map.values_mut() {
                area.receive_mouse_wheel(mouse_pos, movement, mods);
            }
        });
    }

    fn get_on_mouse_enter(&mut self) -> Option<&mut Box<(dyn FnMut(UiId) + 'static)>> {
        None
    }

    fn set_on_mouse_enter(&mut self, f: Box<(dyn FnMut(UiId) + 'static)>) {}

    fn process_event(&mut self, event: Event, mouse_pos: Vector2<f64>) {
        AREA_MAP.with_borrow_mut(|area_map| {
            for area in area_map.values_mut() {
                area.process_event(event, self.mouse_pos);
            }
        });
    }
}

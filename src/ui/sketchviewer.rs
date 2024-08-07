use std::fmt::Debug;

use nalgebra::Vector2;
use raylib::ffi::MouseButton;
use raylib::RaylibHandle;
use raylib::{color::Color, drawing::RaylibDraw};

use raylib::math::Vector2 as V2;

use crate::cad::entity::{Circle, FundamentalEntity, Line};
use crate::combined_draw_handle::CombinedDrawHandle;
use crate::event::Event;
use crate::modes::{ModeId, MousePress};
use crate::rendering::renderer::{to_nalgebra, to_raylib};
use crate::{APP_STATE, EVENT_QUEUE, MODE_STACK};

use super::MouseEventHandler;
use super::{text::Text, Drawable};

struct SketchViewerStyle {
    axis_color: Color,
    entity_color: Color,
    selected_entity_color: Color,
}

impl Default for SketchViewerStyle {
    fn default() -> Self {
        Self {
            axis_color: Color::new(96, 96, 96, 255),
            entity_color: Color::new(238, 238, 238, 255),
            selected_entity_color: Color::new(83, 224, 255, 255),
        }
    }
}

pub struct SketchViewer {
    ui_error: Text,
    pan_offset: Vector2<f64>,
    pos: Vector2<f64>,
    last_mouse_pos: Vector2<f64>,
    scale: f64,
    zoom: f64,
    select_radius: f64,
    texture_size: Vector2<f64>,
    style: SketchViewerStyle,
    panning: bool,
}

impl SketchViewer {
    pub fn new() -> Self {
        let mut ui_error = Text::new();
        ui_error.set_pos(Vector2::new(10.0, 100.0));

        Self {
            ui_error,
            pan_offset: Vector2::new(800.0, 450.0),
            pos: Vector2::zeros(),
            last_mouse_pos: Vector2::zeros(),
            scale: 200.0,
            zoom: 1.0,
            select_radius: 10.0,
            texture_size: Vector2::new(1600.0, 900.0),
            style: SketchViewerStyle::default(),
            panning: false,
        }
    }

    #[inline(always)]
    fn to_screen_space(&self, x: Vector2<f64>) -> V2 {
        to_raylib((x * self.scale * self.zoom) + self.pan_offset)
    }

    #[inline(always)]
    fn to_sketch_space(&self, x: Vector2<f64>) -> Vector2<f64> {
        (x - self.pan_offset) / (self.scale * self.zoom)
    }

    fn to_screen_scale(&self, x: f64) -> f64 {
        x * self.scale * self.zoom
    }

    fn sketch_to_screen_scale(&self, x: f64) -> f64 {
        self.scale * self.zoom
    }

    fn to_sketch_scale(&self, x: f64) -> f64 {
        x / (self.scale * self.zoom)
    }

    pub fn can_ovveride_selection(
        e: &FundamentalEntity,
        selection: &Option<FundamentalEntity>,
    ) -> bool {
        if let Some(selected) = selection {
            matches!(
                (e, selected),
                (FundamentalEntity::Point(_), FundamentalEntity::Point(_))
                    | (FundamentalEntity::Point(_), FundamentalEntity::Line(_))
                    | (FundamentalEntity::Point(_), FundamentalEntity::Circle(_))
                    | (FundamentalEntity::Line(_), FundamentalEntity::Line(_))
                    | (FundamentalEntity::Line(_), FundamentalEntity::Circle(_))
                    | (FundamentalEntity::Circle(_), FundamentalEntity::Line(_))
                    | (FundamentalEntity::Circle(_), FundamentalEntity::Circle(_))
            )
        } else {
            true
        }
    }

    pub fn should_ovveride_selection(
        e: &FundamentalEntity,
        selection: &Option<FundamentalEntity>,
    ) -> bool {
        if let Some(selected) = selection {
            matches!(
                (e, selected),
                (FundamentalEntity::Point(_), FundamentalEntity::Line(_))
                    | (FundamentalEntity::Point(_), FundamentalEntity::Circle(_))
            )
        } else {
            true
        }
    }

    pub fn update_error(&mut self, rl: &mut RaylibHandle) {
        let state = APP_STATE.lock().unwrap();
        self.ui_error
            .set_text(format!("Error: {:?}", state.sketch.error()), rl);
    }

    fn draw_line(
        &self,
        l: &Line,
        rl: &mut CombinedDrawHandle<'_>,
        t: &raylib::prelude::RaylibThread,
        color: Color,
    ) {
        if l.direction.x == 0.0 {
            let offset_screen = self.to_screen_space(l.offset);
            rl.draw_line_v(
                to_raylib(Vector2::new(offset_screen.x as f64, 0.0)),
                to_raylib(Vector2::new(offset_screen.x as f64, self.texture_size.y)),
                color,
            );
        }
        if l.direction.y == 0.0 {
            let offset_screen = self.to_screen_space(l.offset);
            rl.draw_line_v(
                to_raylib(Vector2::new(0.0, offset_screen.y as f64)),
                to_raylib(Vector2::new(self.texture_size.x, offset_screen.y as f64)),
                color,
            );
        }
        let start = self.to_sketch_space(Vector2::zeros());
        let ts = -((l.offset - start).component_div(&l.direction));

        let end = self.to_sketch_space(self.texture_size + self.pan_offset);
        let ts_end = -((l.offset - end).component_div(&l.direction));
        rl.draw_line_v(
            self.to_screen_space(l.offset + l.direction * ts.min()),
            self.to_screen_space(l.offset + l.direction * ts_end.max()),
            color,
        );
    }

    fn draw_circle(
        &self,
        c: &Circle,
        rl: &mut CombinedDrawHandle<'_>,
        t: &raylib::prelude::RaylibThread,
        color: Color,
    ) {
        let pos = self.to_screen_space(c.pos);
        rl.draw_circle_lines(
            pos.x as i32,
            pos.y as i32,
            (c.radius * self.zoom * self.scale) as f32,
            color,
        );
    }
}

impl Drawable for SketchViewer {
    fn move_relative(&mut self, distance: nalgebra::Vector2<f64>) {
        self.ui_error.move_relative(distance);
        self.pos += distance;
    }

    fn set_pos(&mut self, pos: nalgebra::Vector2<f64>) {
        self.move_relative(pos - self.pos)
    }

    fn draw(&self, rl: &mut CombinedDrawHandle<'_>, t: &raylib::prelude::RaylibThread) {
        rl.draw_line_v(
            V2::new(0.0, self.pan_offset.y as f32),
            V2::new(self.texture_size.x as f32, self.pan_offset.y as f32),
            self.style.axis_color,
        );
        rl.draw_line_v(
            V2::new(self.pan_offset.x as f32, 0.0),
            V2::new(self.pan_offset.x as f32, self.texture_size.y as f32),
            self.style.axis_color,
        );

        let state = APP_STATE.lock().unwrap();

        let mut color = self.style.selected_entity_color;
        color.a = 128;
        let ms = MODE_STACK.lock().unwrap();
        if ms.is_innermost_mode(&ModeId::Point) {
            let p = self.to_sketch_space(self.last_mouse_pos);
            rl.draw_circle_v(self.to_screen_space(p), 4.0, color);
        }
        if !state.pending_clicks.is_empty() {
            if ms.is_innermost_mode(&ModeId::Line) {
                let p1 = state.pending_clicks[0];
                let p2 = self.to_sketch_space(self.last_mouse_pos);
                let l = Line {
                    offset: p1,
                    direction: p2 - p1,
                };
                self.draw_line(&l, rl, t, color);
            }
            if ms.is_innermost_mode(&ModeId::Circle) {
                let p1 = state.pending_clicks[0];
                let p2 = self.to_sketch_space(self.last_mouse_pos);
                let c = Circle {
                    pos: p1,
                    radius: (p1 - p2).norm(),
                };
                self.draw_circle(&c, rl, t, color);
            }
        }

        for (id, e) in state.sketch.fundamental_entities.iter() {
            let color = if state.selected.contains(id) {
                self.style.selected_entity_color
            } else {
                self.style.entity_color
            };
            match e {
                FundamentalEntity::Point(p) => {
                    rl.draw_circle_v(self.to_screen_space(p.pos), 4.0, color);
                }
                FundamentalEntity::Line(l) => {
                    self.draw_line(l, rl, t, color);
                }
                FundamentalEntity::Circle(c) => {
                    self.draw_circle(c, rl, t, color);
                }
            }
        }

        self.ui_error.draw(rl, t);
    }

    fn get_size(&self) -> nalgebra::Vector2<f64> {
        Vector2::new(0.0, 0.0)
    }

    fn update(&mut self, rl: &mut RaylibHandle) {
        self.update_error(rl);
    }
}

impl MouseEventHandler for SketchViewer {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        true
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        if self.panning {
            self.pan_offset += mouse_pos - self.last_mouse_pos;
        }
        self.ui_error.receive_mouse_pos(mouse_pos);
        self.last_mouse_pos = mouse_pos;
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &MousePress) {
        let mut eq = EVENT_QUEUE.lock().unwrap();
        match press.button {
            MouseButton::MOUSE_BUTTON_LEFT => {
                eq.post_event(Event::SketchClick {
                    pos: self.to_sketch_space(mouse_pos),
                    sketch_space_select_radius: self.to_sketch_scale(self.select_radius),
                    press: *press,
                });
            }
            MouseButton::MOUSE_BUTTON_RIGHT => {
                eq.post_event(Event::SketchClick {
                    pos: self.to_sketch_space(mouse_pos),
                    sketch_space_select_radius: self.to_sketch_scale(self.select_radius),
                    press: *press,
                });
            }
            MouseButton::MOUSE_BUTTON_MIDDLE => {
                self.panning = true;
            }
            _ => {}
        }
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &MousePress) {
        if press.button == MouseButton::MOUSE_BUTTON_MIDDLE {
            self.panning = false;
        }
    }

    fn process_event(&mut self, event: Event, mouse_pos: Vector2<f64>) {
        if self.contains_point(mouse_pos) {
            match event {
                Event::IncreaseZoom => {
                    let old_offset = self.to_sketch_space(self.pan_offset);
                    self.zoom *= 1.25;
                    self.pan_offset = to_nalgebra(self.to_screen_space(old_offset));
                }
                Event::DecreaseZoom => {
                    let old_offset = self.to_sketch_space(self.pan_offset);
                    self.zoom /= 1.25;
                    self.pan_offset = to_nalgebra(self.to_screen_space(old_offset));
                }
                _ => {}
            }
        }
    }

    fn receive_mouse_wheel(
        &mut self,
        mouse_pos: Vector2<f64>,
        movement: f64,
        mods: &crate::modes::KeyMods,
    ) {
        let mut eq = EVENT_QUEUE.lock().unwrap();
        if mods.ctrl {
            if movement > 0.0 {
                eq.post_event(Event::IncreaseZoom);
            } else if movement < 0.0 {
                eq.post_event(Event::DecreaseZoom);
            }
        }
    }
}

impl Debug for SketchViewer {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("SketchViewer").finish()
    }
}

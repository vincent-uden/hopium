use std::fmt::Debug;

use log::info;
use nalgebra::Vector2;
use raylib::RaylibHandle;
use raylib::{color::Color, drawing::RaylibDraw};

use raylib::math::Vector2 as V2;

use crate::cad::entity::FundamentalEntity;
use crate::event::Event;
use crate::modes::MousePress;
use crate::rendering::renderer::to_raylib;
use crate::{APP_STATE, EVENT_QUEUE};

use super::style::{StyleId, StyleType};
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
}

impl Drawable for SketchViewer {
    fn move_relative(&mut self, distance: nalgebra::Vector2<f64>) {
        self.ui_error.move_relative(distance);
        self.pos += distance;
    }

    fn set_pos(&mut self, pos: nalgebra::Vector2<f64>) {
        self.move_relative(pos - self.pos)
    }

    fn draw(
        &self,
        rl: &mut raylib::prelude::RaylibTextureMode<raylib::prelude::RaylibDrawHandle>,
        t: &raylib::prelude::RaylibThread,
    ) {
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
                FundamentalEntity::Circle(c) => {
                    let pos = self.to_screen_space(c.pos);
                    rl.draw_circle_lines(
                        pos.x as i32,
                        pos.y as i32,
                        (c.radius * self.zoom * self.scale) as f32,
                        color,
                    );
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
        self.ui_error.receive_mouse_pos(mouse_pos);
        let state = APP_STATE.lock().unwrap();
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &MousePress) {
        let mut eq = EVENT_QUEUE.lock().unwrap();
        if press.button == raylib::ffi::MouseButton::MOUSE_BUTTON_LEFT {
            eq.post_event(Event::SketchClick {
                pos: self.to_sketch_space(mouse_pos),
                sketch_space_select_radius: self.to_sketch_scale(self.select_radius),
                press: *press,
            });
        }
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &MousePress) {}
}

impl Debug for SketchViewer {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("SketchViewer").finish()
    }
}

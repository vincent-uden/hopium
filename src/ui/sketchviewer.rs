use std::fmt::Debug;

use nalgebra::Vector2;
use raylib::{color::Color, drawing::RaylibDraw};

use raylib::math::Vector2 as V2;

use crate::cad::entity::FundamentalEntity;
use crate::rendering::renderer::to_raylib;
use crate::APP_STATE;

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
    texture_size: Vector2<f64>,
    style: SketchViewerStyle,
}

impl SketchViewer {
    pub fn new() -> Self {
        Self {
            ui_error: Text::new(),
            pan_offset: Vector2::new(800.0, 450.0),
            pos: Vector2::zeros(),
            last_mouse_pos: Vector2::zeros(),
            scale: 200.0,
            zoom: 1.0,
            texture_size: Vector2::new(1600.0, 900.0),
            style: SketchViewerStyle::default(),
        }
    }

    fn to_screen_space(&self, x: Vector2<f64>) -> V2 {
        to_raylib((x * self.scale * self.zoom) + self.pan_offset)
    }

    fn to_sketch_space(&self, x: Vector2<f64>) -> Vector2<f64> {
        (x - self.pan_offset) / (self.scale * self.zoom)
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
        for e in state.sketch.fundamental_entities.values() {
            match e {
                FundamentalEntity::Point(p) => {
                    rl.draw_circle_v(self.to_screen_space(p.pos), 4.0, self.style.entity_color);
                }
                FundamentalEntity::Line(l) => {
                    let start = self.to_sketch_space(Vector2::zeros());
                    let ts = -((l.offset - start).component_div(&l.direction));

                    let end = self.to_sketch_space(self.texture_size + self.pan_offset);
                    let ts_end = -((l.offset - end).component_div(&l.direction));
                    rl.draw_line_v(
                        self.to_screen_space(l.offset + l.direction * ts.min()),
                        self.to_screen_space(l.offset + l.direction * ts_end.max()),
                        self.style.entity_color,
                    );
                }
                FundamentalEntity::Circle(c) => {
                    let pos = self.to_screen_space(c.pos);
                    rl.draw_circle_lines(
                        pos.x as i32,
                        pos.y as i32,
                        (c.radius * self.zoom * self.scale) as f32,
                        self.style.entity_color,
                    );
                }
            }
        }

        self.ui_error.draw(rl, t);
    }

    fn get_size(&self) -> nalgebra::Vector2<f64> {
        Vector2::new(0.0, 0.0)
    }
}

impl MouseEventHandler for SketchViewer {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        true
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {}

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>) {}

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>) {}
}

impl Debug for SketchViewer {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("SketchViewer").finish()
    }
}

use std::fmt::Debug;

use nalgebra::Vector2;
use raylib::{color::Color, drawing::RaylibDraw};

use raylib::math::Vector2 as V2;

use super::MouseEventHandler;
use super::{text::Text, Drawable};

pub struct SketchViewer {
    ui_error: Text,
    pan_offset: Vector2<f64>,
    pos: Vector2<f64>,
    last_mouse_pos: Vector2<f64>,
    scale: f64,
    zoom: f64,
    texture_size: Vector2<f64>,
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
        }
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
            Color::GREEN,
        );
        rl.draw_line_v(
            V2::new(self.pan_offset.x as f32, 0.0),
            V2::new(self.pan_offset.x as f32, self.texture_size.y as f32),
            Color::GREEN,
        );

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

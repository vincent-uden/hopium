use nalgebra::Vector2;
use raylib::{
    color::Color,
    drawing::{RaylibDraw, RaylibDrawHandle, RaylibTextureMode},
};

use crate::rendering::renderer::to_raylib;

use super::Drawable;

#[derive(Debug)]
struct Rect {
    pos: Vector2<f64>,
    size: Vector2<f64>,
    pub color: Color,
    pub hover_color: Color,
}

impl Rect {
    pub fn new(color: Color) -> Self {
        Self {
            pos: Vector2::<f64>::new(0.0, 0.0),
            size: Vector2::<f64>::new(0.0, 0.0),
            color,
            hover_color: color,
        }
    }
}

impl Drawable for Rect {
    fn move_relative(&mut self, distance: Vector2<f64>) {
        self.pos += distance;
    }

    fn set_pos(&mut self, pos: Vector2<f64>) {
        let diff = pos - self.pos;
        self.move_relative(diff);
    }

    fn draw(&self, rl: &mut RaylibTextureMode<RaylibDrawHandle>, t: &raylib::RaylibThread) {
        rl.draw_rectangle_v(to_raylib(self.pos), to_raylib(self.size), self.color);
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
    }
}

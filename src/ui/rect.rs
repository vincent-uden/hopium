use nalgebra::Vector2;
use raylib::drawing::{RaylibDraw, RaylibDrawHandle, RaylibTextureMode};

use crate::{combined_draw_handle::CombinedDrawHandle, rendering::renderer::to_raylib, STYLES};

use super::{
    style::{StyleId, StyleType},
    Drawable, MouseEventHandler,
};

#[derive(Debug)]
pub struct Rect {
    pub pos: Vector2<f64>,
    pub size: Vector2<f64>,
    pub style: StyleId,
    pub hovered_style: StyleId,
    hovered: bool,
}

impl Rect {
    pub fn new() -> Self {
        Self {
            pos: Vector2::<f64>::new(0.0, 0.0),
            size: Vector2::<f64>::new(0.0, 0.0),
            style: StyleId(StyleType::Default),
            hovered_style: StyleId(StyleType::Default),
            hovered: false,
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

    fn draw(&self, rl: &mut CombinedDrawHandle<'_>, t: &raylib::RaylibThread) {
        let s = &STYLES.read().unwrap()[if self.hovered {
            self.hovered_style
        } else {
            self.style
        }];
        rl.draw_rectangle_v(to_raylib(self.pos), to_raylib(self.size), s.bg_color);
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
    }
}

impl MouseEventHandler for Rect {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        mouse_pos.x > self.pos.x
            && mouse_pos.x < self.pos.x + self.get_size().x
            && mouse_pos.y > self.pos.y
            && mouse_pos.y < self.pos.y + self.get_size().y
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        self.hovered = self.contains_point(mouse_pos);
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {}

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {}
}

use nalgebra::Vector2;
use raylib::{
    color::Color,
    drawing::{RaylibDraw, RaylibDrawHandle, RaylibTextureMode},
    text::RaylibFont,
    RaylibHandle,
};

use crate::{
    rendering::renderer::{to_nalgebra, to_raylib},
    style,
};

use super::Ui;

#[derive(Debug)]
pub enum TextAlignment {
    LEFT,
    CENTER,
    RIGHT,
}

#[derive(Debug)]
pub struct Text {
    pos: Vector2<f64>,
    size: Vector2<f64>,
    pub align: TextAlignment,
    text: String,
    font_size: f32,
}

// TODO: Move off the default font
impl Text {
    pub fn new() -> Self {
        Self {
            pos: Vector2::<f64>::new(0.0, 0.0),
            size: Vector2::<f64>::new(0.0, 0.0),
            align: TextAlignment::LEFT,
            text: "".to_string(),
            font_size: 20.0,
        }
    }

    pub fn set_text(&mut self, text: String, rl: &mut RaylibHandle) {
        self.text = text;
        self.size = to_nalgebra(rl.get_font_default().measure_text(
            &self.text,
            self.font_size,
            1.0,
        ));
    }

    pub fn set_font_size(&mut self, font_size: f32, rl: &mut RaylibHandle) {
        self.font_size = font_size;
        self.size = to_nalgebra(rl.get_font_default().measure_text(
            &self.text,
            self.font_size,
            1.0,
        ));
    }
}

impl Ui for Text {
    fn move_relative(&mut self, distance: Vector2<f64>) {
        self.pos += distance;
    }

    fn set_pos(&mut self, pos: Vector2<f64>) {
        let diff = pos - self.pos;
        self.move_relative(diff);
    }

    fn draw(&self, rl: &mut RaylibTextureMode<RaylibDrawHandle>, t: &raylib::RaylibThread) {
        let mut draw_pos = self.pos;
        match self.align {
            TextAlignment::LEFT => {}
            TextAlignment::CENTER => {
                draw_pos.x -= self.size.x / 2.0;
            }
            TextAlignment::RIGHT => {
                draw_pos.x -= self.size.x;
            }
        }
        let s = style.read().unwrap();
        rl.draw_text_ex(
            rl.get_font_default(),
            &self.text,
            to_raylib(draw_pos),
            self.font_size,
            1.0,
            s.border_color,
        )
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
    }
}

use nalgebra::Vector2;
use raylib::{
    drawing::{RaylibDraw, RaylibDrawHandle, RaylibTextureMode},
    text::RaylibFont,
    RaylibHandle,
};

use crate::{
    rendering::renderer::{to_nalgebra, to_raylib},
    STYLE,
};

use super::{Drawable, MouseEventHandler};

#[derive(Debug)]
pub enum TextAlignment {
    LEFT,
    CENTER,
    RIGHT,
}

pub struct Text {
    pos: Vector2<f64>,
    size: Vector2<f64>,
    pub align: TextAlignment,
    text: String,
    font_size: f32,
    on_mouse_enter: Option<Box<dyn FnMut()>>,
    on_mouse_exit: Option<Box<dyn FnMut()>>,
    hovered: bool,
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
            on_mouse_enter: None,
            on_mouse_exit: None,
            hovered: false,
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

impl Drawable for Text {
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
        let s = STYLE.read().unwrap();
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

impl MouseEventHandler for Text {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        todo!()
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        if self.contains_point(mouse_pos) {
            if !self.hovered {
                if let Some(f) = self.get_on_mouse_enter() {
                    f();
                }
            }
            self.hovered = true;
        } else {
            if self.hovered {
                if let Some(f) = self.get_on_mouse_exit() {
                    f();
                }
            }
            self.hovered = false;
        }
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    fn get_on_mouse_enter(&mut self) -> Option<&mut Box<dyn FnMut()>> {
        self.on_mouse_enter.as_mut()
    }

    fn set_on_mouse_enter(&mut self, f: Box<dyn FnMut()>) {
        self.on_mouse_enter = Some(f);
    }

    fn get_on_mouse_exit(&mut self) -> Option<&mut Box<dyn FnMut()>> {
        self.on_mouse_exit.as_mut()
    }

    fn set_on_mouse_exit(&mut self, f: Box<dyn FnMut()>) {
        self.on_mouse_exit = Some(f);
    }
}

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

use super::{Drawable, MouseEventHandler, UiId};

#[derive(Debug)]
pub enum TextAlignment {
    Left,
    Center,
    Right,
}

pub struct Text {
    pos: Vector2<f64>,
    size: Vector2<f64>,
    pub align: TextAlignment,
    text: String,
    font_size: f32,
    on_mouse_enter: Option<Box<dyn FnMut(UiId)>>,
    on_mouse_exit: Option<Box<dyn FnMut(UiId)>>,
    hovered: bool,
    id: UiId,
}

// TODO: Move off the default font
impl Text {
    pub fn new(id: UiId) -> Self {
        Self {
            pos: Vector2::<f64>::new(0.0, 0.0),
            size: Vector2::<f64>::new(0.0, 0.0),
            align: TextAlignment::Left,
            text: "".to_string(),
            font_size: 20.0,
            on_mouse_enter: None,
            on_mouse_exit: None,
            hovered: false,
            id,
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
            TextAlignment::Left => {}
            TextAlignment::Center => {
                draw_pos.x -= self.size.x / 2.0;
            }
            TextAlignment::Right => {
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
        mouse_pos.x > self.pos.x
            && mouse_pos.x < self.pos.x + self.size.x
            && mouse_pos.y > self.pos.y
            && mouse_pos.y < self.pos.y + self.size.y
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        let id = self.id.clone();
        if self.contains_point(mouse_pos) {
            if !self.hovered {
                if let Some(f) = self.get_on_mouse_enter() {
                    f(id);
                }
            }
            self.hovered = true;
        } else {
            if self.hovered {
                if let Some(f) = self.get_on_mouse_exit() {
                    f(id);
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

    fn get_on_mouse_enter(&mut self) -> Option<&mut Box<(dyn FnMut(UiId) + 'static)>> {
        self.on_mouse_enter.as_mut()
    }

    fn set_on_mouse_enter(&mut self, f: Box<(dyn FnMut(UiId) + 'static)>) {
        self.on_mouse_enter = Some(f);
    }

    fn get_on_mouse_exit(&mut self) -> Option<&mut Box<(dyn FnMut(UiId) + 'static)>> {
        self.on_mouse_exit.as_mut()
    }

    fn set_on_mouse_exit(&mut self, f: Box<dyn FnMut(UiId)>) {
        self.on_mouse_exit = Some(f);
    }
}

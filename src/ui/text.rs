use nalgebra::Vector2;
use raylib::{
    drawing::RaylibDraw,
    text::RaylibFont,
    RaylibHandle,
};

use crate::{
    combined_draw_handle::CombinedDrawHandle,
    rendering::renderer::{to_nalgebra, to_raylib},
    STYLES,
};

use super::{
    style::{StyleId, StyleType},
    Drawable, MouseEventHandler, UiId,
};

#[derive(Debug)]
pub enum TextAlignment {
    Left,
    Center,
    Right,
}

pub struct Text {
    pub pos: Vector2<f64>,
    size: Vector2<f64>,
    pub align: TextAlignment,
    text: String,
    font_size: f32,
    on_mouse_enter: Option<Box<dyn FnMut(UiId)>>,
    on_mouse_exit: Option<Box<dyn FnMut(UiId)>>,
    hovered: bool,
    pub style: StyleId,
    pub hovered_style: StyleId,
}

// TODO: Move off the default font
impl Text {
    pub fn new() -> Self {
        Self {
            pos: Vector2::<f64>::new(0.0, 0.0),
            size: Vector2::<f64>::new(0.0, 0.0),
            align: TextAlignment::Left,
            text: "".to_string(),
            font_size: 20.0,
            on_mouse_enter: None,
            on_mouse_exit: None,
            hovered: false,
            style: StyleId(StyleType::Default),
            hovered_style: StyleId(StyleType::Default),
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

    fn draw(&self, rl: &mut CombinedDrawHandle<'_>, t: &raylib::RaylibThread) {
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
        let s = &STYLES.read().unwrap()[if self.hovered {
            self.hovered_style
        } else {
            self.style
        }];
        rl.draw_text_ex(
            rl.get_font_default(),
            &self.text,
            to_raylib(draw_pos),
            self.font_size,
            1.0,
            s.color,
        )
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
    }
}

impl MouseEventHandler for Text {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        let offset = match self.align {
            TextAlignment::Left => Vector2::<f64>::new(0.0, 0.0),
            TextAlignment::Center => Vector2::<f64>::new(self.size.x / 2.0, 0.0),
            TextAlignment::Right => Vector2::<f64>::new(self.size.x, 0.0),
        };
        let mouse_pos = mouse_pos + offset;
        mouse_pos.x > self.pos.x
            && mouse_pos.x < self.pos.x + self.size.x
            && mouse_pos.y > self.pos.y
            && mouse_pos.y < self.pos.y + self.size.y
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        self.hovered = self.contains_point(mouse_pos);
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {}

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {}

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

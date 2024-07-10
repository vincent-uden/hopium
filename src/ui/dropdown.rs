use nalgebra::Vector2;
use raylib::{
    drawing::{RaylibDrawHandle, RaylibTextureMode},
    text::RaylibFont,
    RaylibHandle, RaylibThread,
};

use crate::rendering::renderer::to_nalgebra;
use crate::ui::Drawable;

use super::{rect::Rect, text::Text, MouseEventHandler};

pub struct DropDown {
    pos: Vector2<f64>,
    btn_size: Vector2<f64>,
    list_size: Vector2<f64>,
    label: String,
    options: Vec<String>,
    hovered: bool,
    font_size: f32,
    padding: f64,
    ui_options: Vec<Text>,
    ui_bgs: Vec<Rect>,
    ui_label: Text,
}

impl DropDown {
    pub fn new() -> Self {
        Self {
            pos: Vector2::zeros(),
            btn_size: Vector2::zeros(),
            list_size: Vector2::zeros(),
            label: String::new(),
            options: Vec::new(),
            hovered: false,
            font_size: 20.0,
            padding: 4.0,
            ui_options: Vec::new(),
            ui_bgs: Vec::new(),
            ui_label: Text::new(),
        }
    }

    pub fn set_contents(&mut self, label: String, options: Vec<String>, rl: &mut RaylibHandle) {
        self.label = label;
        self.options = options;
        self.btn_size = to_nalgebra(rl.get_font_default().measure_text(
            &self.label,
            self.font_size,
            1.0,
        ));

        let mut biggest_size = Vector2::<f64>::zeros();
        for opt in &self.options {
            let size = to_nalgebra(rl.get_font_default().measure_text(opt, self.font_size, 1.0));
            if size.x > biggest_size.x {
                biggest_size = size;
            }
        }
        self.list_size = Vector2::new(
            biggest_size.x + self.padding * 2.0,
            (biggest_size.y + self.padding * 2.0) * self.options.len() as f64,
        );

        self.ui_options.clear();
        self.ui_bgs.clear();

        for (i, opt) in self.options.iter().enumerate() {
            let mut text = Text::new();
            text.set_text(opt.clone(), rl);
            text.set_pos(Vector2::new(
                self.padding,
                (self.padding * 2.0 + biggest_size.y) * (i + 1) as f64 + self.padding,
            ));
            self.ui_options.push(text);
        }

        let mut label = Text::new();
        label.set_text(self.label.clone(), rl);
        label.set_pos(Vector2::new(self.padding, self.padding));
        self.ui_label = label;
    }
}

impl Drawable for DropDown {
    fn move_relative(&mut self, distance: Vector2<f64>) {
        self.pos += distance;
        for ui in &mut self.ui_options {
            ui.move_relative(distance);
        }
    }

    fn set_pos(&mut self, pos: Vector2<f64>) {
        let diff = pos - self.pos;
        self.move_relative(diff);
    }

    fn draw(&self, rl: &mut RaylibTextureMode<RaylibDrawHandle>, t: &RaylibThread) {
        if self.hovered {
            for ui in &self.ui_options {
                ui.draw(rl, t);
            }
        }
        self.ui_label.draw(rl, t);
    }

    fn get_size(&self) -> Vector2<f64> {
        if self.hovered {
            Vector2::new(0.0, self.btn_size.y) + self.list_size
        } else {
            self.btn_size
        }
    }
}

impl MouseEventHandler for DropDown {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        mouse_pos.x > self.pos.x
            && mouse_pos.x < self.pos.x + self.get_size().x
            && mouse_pos.y > self.pos.y
            && mouse_pos.y < self.pos.y + self.get_size().y
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        self.hovered = self.contains_point(mouse_pos);
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }
}

use std::collections::HashMap;

use nalgebra::Vector2;
use nucleo_matcher::{
    pattern::{CaseMatching, Normalization, Pattern},
    Config, Matcher,
};
use raylib::RaylibHandle;

use crate::{
    app::State,
    combined_draw_handle::CombinedDrawHandle,
    event::Event,
    modes::{data_entry::FormInput, ModeId},
    APP_STATE,
};

use super::{
    rect::Rect,
    style::{StyleId, StyleType},
    text::Text,
    Drawable, MouseEventHandler,
};

enum UiFormInput {
    Numerical(Text),
}

pub struct DataEntry {
    pos: Vector2<f64>,
    size: Vector2<f64>,
    bg: Rect,
    header: Text,
    divider: Rect,
    inputs: Vec<UiFormInput>,
    labels: Vec<Text>,
    dividers: Vec<Rect>,
}

impl DataEntry {
    pub fn new(screen_size: Vector2<f64>) -> Self {
        let margin = 10.0;
        let size = Vector2::new(200.0, 400.0);
        let pos = Vector2::new(
            screen_size.x - size.x - margin,
            (screen_size.y - size.y) / 2.0,
        );

        let mut bg = Rect::new();
        bg.size = size;
        bg.style = StyleId(StyleType::DataEntry);
        bg.hovered_style = StyleId(StyleType::DataEntry);
        bg.set_pos(pos);

        let mut divider = Rect::new();
        divider.size = Vector2::new(size.x - 20.0, 2.0);
        divider.set_pos(pos + Vector2::new(10.0, 36.0));
        divider.style = StyleId(StyleType::Divider);
        divider.hovered_style = StyleId(StyleType::Divider);

        let mut header = Text::new();
        header.set_pos(pos + Vector2::new(16.0, 10.0));
        header.style = StyleId(StyleType::DataEntry);
        header.hovered_style = StyleId(StyleType::DataEntry);

        Self {
            pos,
            size,
            bg,
            header,
            divider,
            inputs: vec![],
            labels: vec![],
            dividers: vec![],
        }
    }

    pub fn update(&mut self, rl: &mut RaylibHandle) {
        let state = APP_STATE.lock().unwrap();
        if let Some(form) = &state.form {
            self.header.set_text(form.title.clone(), rl);
            self.inputs.clear();
            for (i, input) in form.entries.iter().enumerate() {
                let mut label = Text::new();
                label.set_pos(self.pos + Vector2::new(16.0, 60.0 + 60.0 * (i as f64)));
                label.style = StyleId(StyleType::DataEntry);
                label.hovered_style = StyleId(StyleType::DataEntry);
                label.set_text(input.label.clone().unwrap(), rl);
                self.labels.push(label);

                match input.input {
                    FormInput::Numerical => {
                        let mut ui_input = Text::new();
                        ui_input
                            .set_pos(self.pos + Vector2::new(16.0, 60.0 + 60.0 * (i as f64 + 0.5)));
                        ui_input.style = StyleId(StyleType::DataEntry);
                        ui_input.set_text(input.buffer.clone(), rl);
                        self.inputs.push(UiFormInput::Numerical(ui_input));
                        let mut divider = Rect::new();
                        divider.size = Vector2::new(self.size.x - 20.0, 2.0);
                        divider
                            .set_pos(self.pos + Vector2::new(10.0, 50.0 + 60.0 * (i as f64 + 1.0)));
                        if state.form_focused == i {
                            divider.style = StyleId(StyleType::DividerFocused);
                            divider.hovered_style = StyleId(StyleType::DividerFocused);
                        } else {
                            divider.style = StyleId(StyleType::Divider);
                            divider.hovered_style = StyleId(StyleType::Divider);
                        }
                        self.dividers.push(divider);
                    }
                }
            }
        }
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
    }
}

impl Drawable for DataEntry {
    fn move_relative(&mut self, distance: Vector2<f64>) {
        self.pos += distance;
        self.bg.move_relative(distance);
        self.header.move_relative(distance);
        for ui in &mut self.inputs {
            match ui {
                UiFormInput::Numerical(ui) => {
                    ui.move_relative(distance);
                }
            }
        }
        for ui in &mut self.labels {
            ui.move_relative(distance);
        }
        for ui in &mut self.dividers {
            ui.move_relative(distance);
        }
    }

    fn set_pos(&mut self, pos: Vector2<f64>) {
        let diff = pos - self.pos;
        self.move_relative(diff);
    }

    fn draw(&self, rl: &mut CombinedDrawHandle<'_>, t: &raylib::prelude::RaylibThread) {
        self.bg.draw(rl, t);
        for ui in &self.inputs {
            match ui {
                UiFormInput::Numerical(ui) => {
                    ui.draw(rl, t);
                }
            }
        }
        for ui in &self.labels {
            ui.draw(rl, t);
        }
        for ui in &self.dividers {
            ui.draw(rl, t);
        }
        self.divider.draw(rl, t);
        self.header.draw(rl, t);
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
    }
}

impl MouseEventHandler for DataEntry {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        true
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {}

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {}

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {}
}

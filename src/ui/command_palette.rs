use nalgebra::Vector2;
use raylib::RaylibHandle;

use crate::event::Event;

use super::{rect::Rect, text::Text, Drawable, MouseEventHandler};

pub struct CommandPalette {
    pos: Vector2<f64>,
    size: Vector2<f64>,
    bg: Rect,
    input: String,
    ui_input: Text,
    suggestions: Vec<Text>,
    suggestion_bgs: Vec<Rect>,
    commands: Vec<String>,
}

impl CommandPalette {
    pub fn new(screen_size: Vector2<f64>) -> Self {
        let size = Vector2::new(800.0, 800.0);
        Self {
            pos: (screen_size - size) / 2.0,
            size,
            bg: Rect::new(),
            input: String::new(),
            ui_input: Text::new(),
            suggestions: vec![],
            suggestion_bgs: vec![],
            commands: vec![],
        }
    }

    pub fn set_commands(&mut self, commands: Vec<String>) {
        self.commands = commands;
    }

    pub fn update(&mut self, input: String, rl: &mut RaylibHandle) {
        self.input = input;
        self.ui_input.set_text(self.input.clone(), rl);
    }

    fn match_commands(&mut self, rl: &mut RaylibHandle) {
        // TODO: Fuzzy find
        let mut candidates = vec![];
        for cmd in &self.commands {
            if cmd.starts_with(&self.input) {
                candidates.push(cmd.clone());
            }
        }

        self.suggestions.clear();
        self.suggestion_bgs.clear();
        for (i, candidate) in candidates[0..(10).min(candidates.len())].iter().enumerate() {
            let mut text = Text::new();
            text.set_text(candidate.clone(), rl);
            text.set_pos(self.pos + Vector2::new(0.0, 30.0 * i as f64));
            self.suggestions.push(text);
        }
    }
}

impl Drawable for CommandPalette {
    fn move_relative(&mut self, distance: Vector2<f64>) {
        self.pos += distance;
        self.bg.move_relative(distance);
        self.ui_input.move_relative(distance);
        for ui in &mut self.suggestions {
            ui.move_relative(distance);
        }
        for ui in &mut self.suggestion_bgs {
            ui.move_relative(distance);
        }
    }

    fn set_pos(&mut self, pos: Vector2<f64>) {
        let diff = pos - self.pos;
        self.move_relative(diff);
    }

    fn draw(
        &self,
        rl: &mut raylib::prelude::RaylibTextureMode<raylib::prelude::RaylibDrawHandle>,
        t: &raylib::prelude::RaylibThread,
    ) {
        self.bg.draw(rl, t);
        self.ui_input.draw(rl, t);
        for ui in &self.suggestions {
            ui.draw(rl, t);
        }
        for ui in &self.suggestion_bgs {
            ui.draw(rl, t);
        }
    }

    fn get_size(&self) -> Vector2<f64> {
        todo!()
    }
}

impl MouseEventHandler for CommandPalette {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        true
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {}

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {}

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {}
}

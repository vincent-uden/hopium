use std::collections::HashMap;

use nalgebra::Vector2;
use nucleo_matcher::{
    pattern::{CaseMatching, Normalization, Pattern},
    Config, Matcher,
};
use raylib::RaylibHandle;

use crate::{
    app::State, combined_draw_handle::CombinedDrawHandle, event::Event, modes::ModeId, APP_STATE,
};

use super::{
    rect::Rect,
    style::{StyleId, StyleType},
    text::Text,
    Drawable, MouseEventHandler,
};

pub struct CommandPalette {
    pos: Vector2<f64>,
    size: Vector2<f64>,
    bg: Rect,
    overlay: Rect,
    divider: Rect,
    input: String,
    ui_input: Text,
    suggestions: Vec<Text>,
    suggestion_bgs: Vec<Rect>,
    commands: HashMap<String, Event>,
    matcher: Matcher,
}

impl CommandPalette {
    pub fn new(screen_size: Vector2<f64>) -> Self {
        let size = Vector2::new(800.0, 800.0);
        let pos = (screen_size - size) / 2.0;

        let mut overlay = Rect::new();
        overlay.size = screen_size;
        overlay.style = StyleId(StyleType::Overlay);
        overlay.hovered_style = StyleId(StyleType::Overlay);
        overlay.set_pos(Vector2::new(0.0, 0.0));

        let mut bg = Rect::new();
        bg.size = size;
        bg.style = StyleId(StyleType::CommandPalette);
        bg.hovered_style = StyleId(StyleType::CommandPalette);
        bg.set_pos(pos);

        let mut ui_input = Text::new();
        ui_input.set_pos(pos + Vector2::new(16.0, 10.0));
        ui_input.style = StyleId(StyleType::CommandPalette);
        ui_input.hovered_style = StyleId(StyleType::CommandPalette);

        let mut divider = Rect::new();
        divider.size = Vector2::new(size.y - 20.0, 2.0);
        divider.set_pos(pos + Vector2::new(10.0, 36.0));
        divider.style = StyleId(StyleType::CommandPaletteDivider);
        divider.hovered_style = StyleId(StyleType::CommandPaletteDivider);

        let mut commands = HashMap::new();
        commands.insert(
            "Enter point mode".to_string(),
            Event::PushMode(ModeId::Point),
        );
        commands.insert("Enter line mode".to_string(), Event::PushMode(ModeId::Line));
        commands.insert(
            "Enter circle mode".to_string(),
            Event::PushMode(ModeId::Circle),
        );
        commands.insert(
            "Enter sketch mode".to_string(),
            Event::PushMode(ModeId::Sketch),
        );
        commands.insert(
            "Enter capped line mode".to_string(),
            Event::PushMode(ModeId::CappedLine),
        );
        commands.insert(
            "Enter three point arc mode".to_string(),
            Event::PushMode(ModeId::ArcThreePoint),
        );
        commands.insert("Dump state".to_string(), Event::DumpState);

        let matcher = Matcher::new(Config::DEFAULT);

        Self {
            pos,
            size,
            bg,
            overlay,
            divider,
            input: String::new(),
            ui_input,
            suggestions: vec![],
            suggestion_bgs: vec![],
            commands,
            matcher,
        }
    }

    pub fn update(&mut self, rl: &mut RaylibHandle) {
        let mut state = APP_STATE.lock().unwrap();
        self.input = state.command_palette_input.clone();
        self.ui_input.set_text(self.input.clone(), rl);
        self.match_commands(rl, &mut state);
    }

    fn match_commands(&mut self, rl: &mut RaylibHandle, app_state: &mut State) {
        let matches = Pattern::parse(&self.input, CaseMatching::Ignore, Normalization::Smart)
            .match_list(self.commands.keys(), &mut self.matcher);

        self.suggestions.clear();
        self.suggestion_bgs.clear();
        for (i, (candidate, score)) in matches[0..(10).min(matches.len())].iter().enumerate() {
            let mut text = Text::new();
            text.set_text(candidate.to_string().clone(), rl);
            text.set_pos(self.pos + Vector2::new(16.0, 60.0 + 30.0 * i as f64));
            text.style = StyleId(StyleType::CommandPalette);
            text.hovered_style = StyleId(StyleType::CommandPalette);
            self.suggestions.push(text);
        }

        if !matches.is_empty() {
            app_state.command_palette_pending_event = Some(self.commands[matches[0].0]);
        } else {
            app_state.command_palette_pending_event = None;
        }
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
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

    fn draw(&self, rl: &mut CombinedDrawHandle<'_>, t: &raylib::prelude::RaylibThread) {
        self.overlay.draw(rl, t);
        self.bg.draw(rl, t);
        for ui in &self.suggestions {
            ui.draw(rl, t);
        }
        for ui in &self.suggestion_bgs {
            ui.draw(rl, t);
        }
        self.ui_input.draw(rl, t);
        self.divider.draw(rl, t);
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
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

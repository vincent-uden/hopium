use nalgebra::Vector2;
use raylib::ffi::KeyboardKey;

use crate::{
    event::Event,
    APP_STATE, EVENT_QUEUE,
};

use super::{Mode, ModeId, MousePress};

#[derive(Debug)]
pub struct CommandMode {}

impl CommandMode {
    pub fn new() -> Self {
        Self {}
    }

    fn sketch_click(&self, click_pos: Vector2<f64>, press: MousePress) {}
}

impl Mode for CommandMode {
    fn id(&self) -> ModeId {
        ModeId::Command
    }

    fn process_event(&self, event: crate::event::Event) -> bool {
        false
    }

    fn key_press(&mut self, key: &super::KeyPress, rl: &mut raylib::RaylibHandle) -> bool {
        let mut state = APP_STATE.lock().unwrap();
        let mut eq = EVENT_QUEUE.lock().unwrap();
        match key.key {
            KeyboardKey::KEY_ESCAPE => {
                eq.post_event(Event::PopMode);
                state.command_palette_open = false;
            }
            KeyboardKey::KEY_BACKSPACE => {
                if key.ctrl {
                    if let Some(idx) = state.command_palette_input.rfind(' ') {
                        state.command_palette_input.drain(idx..);
                    } else {
                        state.command_palette_input.clear();
                    }
                }
                state.command_palette_input.pop();
            }
            KeyboardKey::KEY_ENTER => {
                if let Some(event) = state.command_palette_pending_event.take() {
                    state.command_palette_open = false;
                    eq.post_event(Event::PopMode);
                    eq.post_event(event);
                }
            }
            _ => {}
        }
        if let Some(ch) = key.char() {
            state.command_palette_input.push(ch);
        }
        true
    }
}

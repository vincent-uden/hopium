use nalgebra::Vector2;
use raylib::ffi::KeyboardKey;

use crate::{event::Event, APP_STATE, EVENT_QUEUE};

use super::{Mode, ModeId, MousePress};

#[derive(Debug)]
pub struct FormEntry {
    pub label: Option<String>,
    pub input: FormInput,
    pub buffer: String,
}

#[derive(Debug)]
pub enum FormInput {
    Numerical,
}

#[derive(Debug, Default)]
pub struct Form {
    pub title: String,
    pub entries: Vec<FormEntry>,
}
impl Form {
    pub fn example() -> Form {
        Form {
            title: "Example Form".to_string(),
            entries: vec![
                FormEntry {
                    label: Some("Length".to_string()),
                    input: FormInput::Numerical,
                    buffer: "".to_string(),
                },
                FormEntry {
                    label: Some("Width".to_string()),
                    input: FormInput::Numerical,
                    buffer: "".to_string(),
                },
                FormEntry {
                    label: Some("Depth".to_string()),
                    input: FormInput::Numerical,
                    buffer: "".to_string(),
                },
            ],
        }
    }

    pub fn angle() -> Form {
        Form {
            title: "Constrain".to_string(),
            entries: vec![FormEntry {
                label: Some("Angle".to_string()),
                input: FormInput::Numerical,
                buffer: "".to_string(),
            }],
        }
    }

    pub fn distance() -> Form {
        Form {
            title: "Constrain".to_string(),
            entries: vec![FormEntry {
                label: Some("Distance".to_string()),
                input: FormInput::Numerical,
                buffer: "".to_string(),
            }],
        }
    }
}

#[derive(Debug)]
pub struct DataEntryMode {}

impl DataEntryMode {
    pub fn new() -> Self {
        Self {}
    }

    fn sketch_click(&self, click_pos: Vector2<f64>, press: MousePress) {}
}

impl Mode for DataEntryMode {
    fn id(&self) -> ModeId {
        ModeId::DataEntry
    }

    fn process_event(&self, event: crate::event::Event) -> bool {
        false
    }

    fn key_press(&mut self, key: &super::KeyPress, rl: &mut raylib::RaylibHandle) -> bool {
        let mut state = APP_STATE.lock().unwrap();
        let mut eq = EVENT_QUEUE.lock().unwrap();
        let mut consumed = true;
        match key.key {
            KeyboardKey::KEY_ESCAPE => {
                eq.post_event(Event::PopMode);
                state.form = None;
            }
            KeyboardKey::KEY_BACKSPACE => {}
            KeyboardKey::KEY_ENTER => {
                eq.post_event(Event::ConfirmDataEntry);
            }
            KeyboardKey::KEY_TAB => match &state.form {
                Some(form) => {
                    if key.shift && state.form_focused > 0 {
                        state.form_focused -= 1;
                    } else if state.form_focused < form.entries.len() {
                        state.form_focused += 1;
                    }
                }
                None => {}
            },
            _ => {
                consumed = false;
            }
        }
        if let Some(ch) = key.char() {
            let focused = state.form_focused;
            if let Some(form) = &mut state.form {
                form.entries[focused].buffer.push(ch);
                consumed = true;
            }
        }
        consumed
    }
}

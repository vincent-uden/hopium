use nalgebra::Vector2;
use raylib::{ffi::KeyboardKey, RaylibHandle};

use crate::{event::Event, EVENT_QUEUE};

use super::{Mode, ModeId};

#[derive(Debug)]
pub struct GlobalMode {}

impl GlobalMode {
    pub fn new() -> Self {
        Self {}
    }
}

impl Mode for GlobalMode {
    fn id(&self) -> super::ModeId {
        ModeId::Global
    }

    fn process_event(&self, event: Event) -> bool {
        false
    }

    fn key_press(&mut self, key: &super::KeyPress, rl: &mut RaylibHandle) -> bool {
        let mut consumed = true;
        let mut eq = EVENT_QUEUE.lock().unwrap();
        match key.key {
            KeyboardKey::KEY_H => eq.post_event(Event::SplitPaneHorizontally {
                mouse_pos: Vector2::<f64>::new(rl.get_mouse_x() as f64, rl.get_mouse_y() as f64),
            }),
            KeyboardKey::KEY_V => eq.post_event(Event::SplitPaneVertically {
                mouse_pos: Vector2::<f64>::new(rl.get_mouse_x() as f64, rl.get_mouse_y() as f64),
            }),
            KeyboardKey::KEY_D => eq.post_event(Event::CollapseBoundary {
                mouse_pos: Vector2::<f64>::new(rl.get_mouse_x() as f64, rl.get_mouse_y() as f64),
            }),
            KeyboardKey::KEY_Q => eq.post_event(Event::ExitProgram),
            KeyboardKey::KEY_S => {
                if key.ctrl {
                    eq.post_event(Event::SaveSketch);
                } else {
                    eq.post_event(Event::PushMode(ModeId::Sketch));
                }
            }
            KeyboardKey::KEY_O => {
                if key.ctrl {
                    eq.post_event(Event::LoadSketch);
                }
            }
            KeyboardKey::KEY_W => eq.post_event(Event::DumpShapes),
            KeyboardKey::KEY_P => {
                if key.ctrl {
                    eq.post_event(Event::OpenCommandPalette);
                }
            }
            _ => {
                consumed = false;
            }
        }
        consumed
    }
}

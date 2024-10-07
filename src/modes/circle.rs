use nalgebra::Vector2;
use raylib::ffi::{KeyboardKey, MouseButton};

use crate::{
    cad::entity::{Circle, FundamentalEntity},
    event::Event,
    APP_STATE, EVENT_QUEUE,
};

use super::{Mode, ModeId, MouseButtonDef, MousePress};

#[derive(Debug)]
pub struct CircleMode {}

impl CircleMode {
    pub fn new() -> Self {
        Self {}
    }

    fn sketch_click(&self, click_pos: Vector2<f64>, press: MousePress) {
        let mut state = APP_STATE.lock().unwrap();
        if press.button == raylib::ffi::MouseButton::MOUSE_BUTTON_LEFT {
            state.pending_clicks.push(click_pos);
        } else if press.button == raylib::ffi::MouseButton::MOUSE_BUTTON_RIGHT {
            state.pending_clicks.clear();
        }

        if state.pending_clicks.len() == 2 {
            let p1 = state.pending_clicks[0];
            let p2 = state.pending_clicks[1];
            state.pending_clicks.clear();
            state
                .sketch
                .fundamental_entities
                .insert(FundamentalEntity::Circle(Circle {
                    pos: p1,
                    radius: (p1 - p2).norm(),
                }));
        }
    }
}

impl Mode for CircleMode {
    fn id(&self) -> ModeId {
        ModeId::Circle
    }

    fn process_event(&self, event: crate::event::Event) -> bool {
        match event {
            Event::SketchClick {
                pos,
                sketch_space_select_radius,
                press,
            } => {
                self.sketch_click(pos, press);
                true
            }
            _ => false,
        }
    }

    fn key_press(&mut self, key: &super::KeyPress, rl: &mut raylib::RaylibHandle) -> bool {
        let mut eq = EVENT_QUEUE.lock().unwrap();
        let mut out = true;
        match key.key {
            KeyboardKey::KEY_ESCAPE => {
                eq.post_event(Event::PopMode);
            }
            _ => {
                out = false;
            }
        };
        out
    }

    fn mouse_press(&mut self, key: &super::MousePress) -> bool {
        let mut eq = EVENT_QUEUE.lock().unwrap();
        let mut out = true;
        match key.button {
            MouseButton::MOUSE_BUTTON_RIGHT => {
                eq.post_event(Event::PopMode);
            }
            _ => {
                out = false;
            }
        }
        out
    }
}

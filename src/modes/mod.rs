use std::{collections::HashMap, fmt::Debug};

use log::debug;
use raylib::{
    ffi::{KeyboardKey, MouseButton},
    RaylibHandle,
};
use serde::{Deserialize, Serialize};
use strum_macros::EnumString;

use crate::{event::Event, APP_STATE};

pub mod arc_three_point;
pub mod capped_line;
pub mod circle;
pub mod command;
pub mod data_entry;
pub mod global;
pub mod line;
pub mod point;
pub mod sketch;

#[derive(Debug, Clone, Copy)]
pub struct KeyPress {
    pub key: KeyboardKey,
    pub shift: bool,
    pub ctrl: bool,
    pub l_alt: bool,
    pub r_alt: bool,
}

impl KeyPress {
    pub fn char(&self) -> Option<char> {
        match self.key {
            KeyboardKey::KEY_COMMA => Some(','),
            KeyboardKey::KEY_MINUS => Some('-'),
            KeyboardKey::KEY_PERIOD => Some('.'),
            KeyboardKey::KEY_SLASH => Some('/'),
            KeyboardKey::KEY_ZERO => Some('0'),
            KeyboardKey::KEY_ONE => Some('1'),
            KeyboardKey::KEY_TWO => Some('2'),
            KeyboardKey::KEY_THREE => Some('3'),
            KeyboardKey::KEY_FOUR => Some('4'),
            KeyboardKey::KEY_FIVE => Some('5'),
            KeyboardKey::KEY_SIX => Some('6'),
            KeyboardKey::KEY_SEVEN => Some('7'),
            KeyboardKey::KEY_EIGHT => Some('8'),
            KeyboardKey::KEY_NINE => Some('9'),
            KeyboardKey::KEY_SEMICOLON => Some(';'),
            KeyboardKey::KEY_EQUAL => Some('='),
            KeyboardKey::KEY_A => Some('a'),
            KeyboardKey::KEY_B => Some('b'),
            KeyboardKey::KEY_C => Some('c'),
            KeyboardKey::KEY_D => Some('d'),
            KeyboardKey::KEY_E => Some('e'),
            KeyboardKey::KEY_F => Some('f'),
            KeyboardKey::KEY_G => Some('g'),
            KeyboardKey::KEY_H => Some('h'),
            KeyboardKey::KEY_I => Some('i'),
            KeyboardKey::KEY_J => Some('j'),
            KeyboardKey::KEY_K => Some('k'),
            KeyboardKey::KEY_L => Some('l'),
            KeyboardKey::KEY_M => Some('m'),
            KeyboardKey::KEY_N => Some('n'),
            KeyboardKey::KEY_O => Some('o'),
            KeyboardKey::KEY_P => Some('p'),
            KeyboardKey::KEY_Q => Some('q'),
            KeyboardKey::KEY_R => Some('r'),
            KeyboardKey::KEY_S => Some('s'),
            KeyboardKey::KEY_T => Some('t'),
            KeyboardKey::KEY_U => Some('u'),
            KeyboardKey::KEY_V => Some('v'),
            KeyboardKey::KEY_W => Some('w'),
            KeyboardKey::KEY_X => Some('x'),
            KeyboardKey::KEY_Y => Some('y'),
            KeyboardKey::KEY_Z => Some('z'),
            KeyboardKey::KEY_LEFT_BRACKET => Some('['),
            KeyboardKey::KEY_BACKSLASH => Some('\\'),
            KeyboardKey::KEY_RIGHT_BRACKET => Some(']'),
            KeyboardKey::KEY_SPACE => Some(' '),
            KeyboardKey::KEY_KP_0 => Some('0'),
            KeyboardKey::KEY_KP_1 => Some('1'),
            KeyboardKey::KEY_KP_2 => Some('2'),
            KeyboardKey::KEY_KP_3 => Some('3'),
            KeyboardKey::KEY_KP_4 => Some('4'),
            KeyboardKey::KEY_KP_5 => Some('5'),
            KeyboardKey::KEY_KP_6 => Some('6'),
            KeyboardKey::KEY_KP_7 => Some('7'),
            KeyboardKey::KEY_KP_8 => Some('8'),
            KeyboardKey::KEY_KP_9 => Some('9'),
            KeyboardKey::KEY_KP_DECIMAL => Some('.'),
            KeyboardKey::KEY_KP_DIVIDE => Some('/'),
            KeyboardKey::KEY_KP_MULTIPLY => Some('*'),
            KeyboardKey::KEY_KP_SUBTRACT => Some('-'),
            KeyboardKey::KEY_KP_ADD => Some('+'),
            KeyboardKey::KEY_KP_ENTER => Some('\n'),
            KeyboardKey::KEY_KP_EQUAL => Some('='),
            _ => None,
        }
    }

    pub fn vanilla(&self) -> bool {
        !(self.ctrl || self.l_alt || self.r_alt || self.shift)
    }
}

#[allow(non_camel_case_types)]
#[derive(Deserialize, Serialize)]
#[serde(remote = "MouseButton")]
enum MouseButtonDef {
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_RIGHT = 1,
    MOUSE_BUTTON_MIDDLE = 2,
    MOUSE_BUTTON_SIDE = 3,
    MOUSE_BUTTON_EXTRA = 4,
    MOUSE_BUTTON_FORWARD = 5,
    MOUSE_BUTTON_BACK = 6,
}

#[derive(Debug, Clone, Copy, Deserialize, Serialize)]
pub struct MousePress {
    #[serde(with = "MouseButtonDef")]
    pub button: MouseButton,
    pub shift: bool,
    pub ctrl: bool,
    pub l_alt: bool,
    pub r_alt: bool,
}

#[derive(Debug, Clone, Copy, Deserialize, Serialize)]
pub struct KeyMods {
    pub shift: bool,
    pub ctrl: bool,
    pub l_alt: bool,
    pub r_alt: bool,
}

#[derive(Debug, PartialEq, Eq, EnumString, Serialize, Deserialize, Clone, Copy, Hash)]
pub enum ModeId {
    Global,
    Sketch,
    Point,
    Line,
    Circle,
    Command,
    CappedLine,
    ArcThreePoint,
    DataEntry,
}

pub trait Mode {
    fn id(&self) -> ModeId;
    /// Due to ownership rules, a mode may not directly modify the mode stack.
    fn process_event(&self, event: Event) -> bool;

    fn key_press(&mut self, key: &KeyPress, rl: &mut RaylibHandle) -> bool {
        false
    }

    fn key_release(&mut self, key: &KeyPress) -> bool {
        false
    }

    fn mouse_press(&mut self, key: &MousePress) -> bool {
        false
    }

    fn mouse_release(&mut self, key: &MousePress) -> bool {
        false
    }
}

pub struct ModeStack {
    modes: Vec<ModeId>,
    pub all_modes: HashMap<ModeId, Box<dyn Mode + Send + Sync>>,
    pub all_keys: Vec<KeyboardKey>,
    pub all_mouse_buttons: Vec<MouseButton>,
}

impl ModeStack {
    pub fn new() -> Self {
        Self {
            modes: vec![],
            all_keys: vec![
                KeyboardKey::KEY_NULL,
                KeyboardKey::KEY_APOSTROPHE,
                KeyboardKey::KEY_COMMA,
                KeyboardKey::KEY_MINUS,
                KeyboardKey::KEY_PERIOD,
                KeyboardKey::KEY_SLASH,
                KeyboardKey::KEY_ZERO,
                KeyboardKey::KEY_ONE,
                KeyboardKey::KEY_TWO,
                KeyboardKey::KEY_THREE,
                KeyboardKey::KEY_FOUR,
                KeyboardKey::KEY_FIVE,
                KeyboardKey::KEY_SIX,
                KeyboardKey::KEY_SEVEN,
                KeyboardKey::KEY_EIGHT,
                KeyboardKey::KEY_NINE,
                KeyboardKey::KEY_SEMICOLON,
                KeyboardKey::KEY_EQUAL,
                KeyboardKey::KEY_A,
                KeyboardKey::KEY_B,
                KeyboardKey::KEY_C,
                KeyboardKey::KEY_D,
                KeyboardKey::KEY_E,
                KeyboardKey::KEY_F,
                KeyboardKey::KEY_G,
                KeyboardKey::KEY_H,
                KeyboardKey::KEY_I,
                KeyboardKey::KEY_J,
                KeyboardKey::KEY_K,
                KeyboardKey::KEY_L,
                KeyboardKey::KEY_M,
                KeyboardKey::KEY_N,
                KeyboardKey::KEY_O,
                KeyboardKey::KEY_P,
                KeyboardKey::KEY_Q,
                KeyboardKey::KEY_R,
                KeyboardKey::KEY_S,
                KeyboardKey::KEY_T,
                KeyboardKey::KEY_U,
                KeyboardKey::KEY_V,
                KeyboardKey::KEY_W,
                KeyboardKey::KEY_X,
                KeyboardKey::KEY_Y,
                KeyboardKey::KEY_Z,
                KeyboardKey::KEY_LEFT_BRACKET,
                KeyboardKey::KEY_BACKSLASH,
                KeyboardKey::KEY_RIGHT_BRACKET,
                KeyboardKey::KEY_GRAVE,
                KeyboardKey::KEY_SPACE,
                KeyboardKey::KEY_ESCAPE,
                KeyboardKey::KEY_ENTER,
                KeyboardKey::KEY_TAB,
                KeyboardKey::KEY_BACKSPACE,
                KeyboardKey::KEY_INSERT,
                KeyboardKey::KEY_DELETE,
                KeyboardKey::KEY_RIGHT,
                KeyboardKey::KEY_LEFT,
                KeyboardKey::KEY_DOWN,
                KeyboardKey::KEY_UP,
                KeyboardKey::KEY_PAGE_UP,
                KeyboardKey::KEY_PAGE_DOWN,
                KeyboardKey::KEY_HOME,
                KeyboardKey::KEY_END,
                KeyboardKey::KEY_CAPS_LOCK,
                KeyboardKey::KEY_SCROLL_LOCK,
                KeyboardKey::KEY_NUM_LOCK,
                KeyboardKey::KEY_PRINT_SCREEN,
                KeyboardKey::KEY_PAUSE,
                KeyboardKey::KEY_F1,
                KeyboardKey::KEY_F2,
                KeyboardKey::KEY_F3,
                KeyboardKey::KEY_F4,
                KeyboardKey::KEY_F5,
                KeyboardKey::KEY_F6,
                KeyboardKey::KEY_F7,
                KeyboardKey::KEY_F8,
                KeyboardKey::KEY_F9,
                KeyboardKey::KEY_F10,
                KeyboardKey::KEY_F11,
                KeyboardKey::KEY_F12,
                KeyboardKey::KEY_LEFT_SHIFT,
                KeyboardKey::KEY_LEFT_CONTROL,
                KeyboardKey::KEY_LEFT_ALT,
                KeyboardKey::KEY_LEFT_SUPER,
                KeyboardKey::KEY_RIGHT_SHIFT,
                KeyboardKey::KEY_RIGHT_CONTROL,
                KeyboardKey::KEY_RIGHT_ALT,
                KeyboardKey::KEY_RIGHT_SUPER,
                KeyboardKey::KEY_KB_MENU,
                KeyboardKey::KEY_KP_0,
                KeyboardKey::KEY_KP_1,
                KeyboardKey::KEY_KP_2,
                KeyboardKey::KEY_KP_3,
                KeyboardKey::KEY_KP_4,
                KeyboardKey::KEY_KP_5,
                KeyboardKey::KEY_KP_6,
                KeyboardKey::KEY_KP_7,
                KeyboardKey::KEY_KP_8,
                KeyboardKey::KEY_KP_9,
                KeyboardKey::KEY_KP_DECIMAL,
                KeyboardKey::KEY_KP_DIVIDE,
                KeyboardKey::KEY_KP_MULTIPLY,
                KeyboardKey::KEY_KP_SUBTRACT,
                KeyboardKey::KEY_KP_ADD,
                KeyboardKey::KEY_KP_ENTER,
                KeyboardKey::KEY_KP_EQUAL,
                KeyboardKey::KEY_BACK,
                KeyboardKey::KEY_VOLUME_UP,
                KeyboardKey::KEY_VOLUME_DOWN,
            ],
            all_mouse_buttons: vec![
                MouseButton::MOUSE_BUTTON_LEFT,
                MouseButton::MOUSE_BUTTON_RIGHT,
                MouseButton::MOUSE_BUTTON_MIDDLE,
                MouseButton::MOUSE_BUTTON_SIDE,
                MouseButton::MOUSE_BUTTON_EXTRA,
                MouseButton::MOUSE_BUTTON_FORWARD,
                MouseButton::MOUSE_BUTTON_BACK,
            ],
            all_modes: HashMap::new(),
        }
    }

    /// Due to ownership rules, a mode may not directly modify the mode stack.
    pub fn process_event(&mut self, event: Event) -> bool {
        let mut consumed = false;
        for id in self.modes.iter().rev() {
            let mode = &self.all_modes[id];
            consumed = consumed || mode.process_event(event);
        }
        let mut state = APP_STATE.lock().unwrap();
        if !consumed {
            consumed = true;
            match event {
                Event::PopMode => {
                    state.pending_clicks.clear();
                    self.modes.pop();
                }
                Event::PushMode(mode_id) => {
                    self.modes.push(mode_id);
                }
                Event::SwitchMode {
                    switch_after,
                    switch_to,
                } => {
                    while self.innermost_mode() != switch_after && self.len() > 1 {
                        self.pop();
                    }
                    self.push(switch_to);
                }
                _ => {
                    consumed = false;
                }
            }
        }
        consumed
    }

    pub fn exit(&mut self, mode_id: &ModeId) {
        if self.is_active(mode_id) {
            while self
                .modes
                .pop()
                .expect("Since the mode is active, self.modes CANT be empty")
                != *mode_id
            {}
        }
    }

    pub fn update(&mut self, rl: &mut RaylibHandle) {
        let shift = rl.is_key_down(KeyboardKey::KEY_LEFT_SHIFT)
            || rl.is_key_down(KeyboardKey::KEY_RIGHT_SHIFT);
        let ctrl = rl.is_key_down(KeyboardKey::KEY_LEFT_CONTROL)
            || rl.is_key_down(KeyboardKey::KEY_RIGHT_CONTROL);
        let l_alt = rl.is_key_down(KeyboardKey::KEY_LEFT_ALT);
        let r_alt = rl.is_key_down(KeyboardKey::KEY_RIGHT_ALT);

        for key in &self.all_keys {
            if rl.is_key_pressed(*key) {
                let press = KeyPress {
                    key: *key,
                    shift,
                    ctrl,
                    l_alt,
                    r_alt,
                };
                for id in self.modes.iter_mut().rev() {
                    let mode = self.all_modes.get_mut(id).unwrap();
                    if mode.key_press(&press, rl) {
                        break;
                    }
                }
            }
            if rl.is_key_released(*key) {
                let press = KeyPress {
                    key: *key,
                    shift,
                    ctrl,
                    l_alt,
                    r_alt,
                };
                for id in self.modes.iter_mut().rev() {
                    let mode = self.all_modes.get_mut(id).unwrap();
                    if mode.key_release(&press) {
                        break;
                    }
                }
            }
        }

        for button in &self.all_mouse_buttons {
            if rl.is_mouse_button_pressed(*button) {
                let press = MousePress {
                    button: *button,
                    shift,
                    ctrl,
                    l_alt,
                    r_alt,
                };
                for id in self.modes.iter_mut().rev() {
                    let mode = self.all_modes.get_mut(id).unwrap();
                    if mode.mouse_press(&press) {
                        break;
                    }
                }
            }
            if rl.is_mouse_button_released(*button) {
                let press = MousePress {
                    button: *button,
                    shift,
                    ctrl,
                    l_alt,
                    r_alt,
                };
                for id in self.modes.iter_mut().rev() {
                    let mode = self.all_modes.get_mut(id).unwrap();
                    if mode.mouse_release(&press) {
                        break;
                    }
                }
            }
        }
    }

    pub fn push(&mut self, mode: ModeId) {
        self.modes.push(mode);
    }

    pub fn pop(&mut self) {
        self.modes.pop();
    }

    pub fn len(&self) -> usize {
        self.modes.len()
    }

    pub fn is_active(&self, mode_id: &ModeId) -> bool {
        for id in &self.modes {
            if id == mode_id {
                return true;
            }
        }
        false
    }

    pub fn is_innermost_mode(&self, mode_id: &ModeId) -> bool {
        match self.modes.last() {
            Some(id) => id == mode_id,
            None => false,
        }
    }

    pub fn innermost_mode(&self) -> ModeId {
        *self.modes.last().unwrap()
    }
}

impl Debug for ModeStack {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("ModeStack")
            .field("modes", &self.modes.len())
            .finish()
    }
}

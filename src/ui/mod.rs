use std::hash::Hash;

use nalgebra::Vector2;
use raylib::{
    drawing::{RaylibDrawHandle, RaylibTextureMode},
    RaylibHandle, RaylibThread,
};
use serde::{Deserialize, Serialize};

use crate::{
    combined_draw_handle::CombinedDrawHandle,
    event::Event,
    modes::{KeyMods, MousePress},
    registry::RegId,
};

pub mod command_palette;
pub mod constraint_selector;
pub mod dropdown;
pub mod icon;
pub mod rect;
pub mod sketchviewer;
pub mod style;
pub mod text;

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy, Serialize, Deserialize)]
pub struct UiId(pub i64);

impl RegId for UiId {
    fn new() -> Self {
        Self(0)
    }

    fn increment(self) -> Self {
        let UiId(id) = self;
        Self(id + 1)
    }
}

pub trait Drawable {
    fn move_relative(&mut self, distance: Vector2<f64>);
    fn set_pos(&mut self, pos: Vector2<f64>);
    fn draw(&self, rl: &mut CombinedDrawHandle, t: &RaylibThread);
    fn get_size(&self) -> Vector2<f64>;
    fn update(&mut self, rl: &mut RaylibHandle) {}
}

pub trait MouseEventHandler {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool;

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>);

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &MousePress);

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &MousePress);

    fn receive_mouse_wheel(&mut self, mouse_pos: Vector2<f64>, movement: f64, mods: &KeyMods) {}

    fn get_on_mouse_enter(&mut self) -> Option<&mut Box<dyn FnMut(UiId)>> {
        None
    }

    fn set_on_mouse_enter(&mut self, f: Box<dyn FnMut(UiId)>) {}

    fn get_on_mouse_exit(&mut self) -> Option<&mut Box<dyn FnMut(UiId)>> {
        None
    }

    fn set_on_mouse_exit(&mut self, f: Box<dyn FnMut(UiId)>) {}

    fn process_event(&mut self, event: Event, mouse_pos: Vector2<f64>) {}
}

pub trait Ui: Drawable + MouseEventHandler {}
impl<T: Drawable + MouseEventHandler> Ui for T {}

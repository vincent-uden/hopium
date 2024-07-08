use nalgebra::Vector2;
use raylib::{
    drawing::{RaylibDrawHandle, RaylibTextureMode},
    RaylibThread,
};

pub mod style;
pub mod text;

pub trait Ui {
    fn move_relative(&mut self, distance: Vector2<f64>);
    fn set_pos(&mut self, distance: Vector2<f64>);
    fn draw(&self, rl: &mut RaylibTextureMode<RaylibDrawHandle>, t: &RaylibThread);
    fn get_size(&self) -> Vector2<f64>;
}

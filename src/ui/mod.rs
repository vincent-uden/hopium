use std::{
    cell::RefCell,
    collections::{
        hash_map::{Iter, Values, ValuesMut},
        HashMap,
    },
    hash::Hash,
    ops::{Index, IndexMut},
};

use nalgebra::Vector2;
use raylib::{
    drawing::{RaylibDrawHandle, RaylibTextureMode},
    RaylibThread,
};

pub mod rect;
pub mod style;
pub mod text;

pub struct Registry<K: RegId + Eq + Hash, V> {
    map: HashMap<K, V>,
    next_id: K,
}

impl<K: RegId + Eq + Hash, V> Registry<K, V> {
    pub fn new() -> Self {
        Self {
            map: HashMap::new(),
            next_id: K::new(),
        }
    }

    #[inline(always)]
    pub fn insert(&mut self, k: K, v: V) {
        self.map.insert(k, v);
    }

    #[inline(always)]
    pub fn clear(&mut self) {
        self.map.clear();
    }

    #[inline(always)]
    pub fn values(&self) -> Values<K, V> {
        self.map.values()
    }

    #[inline(always)]
    pub fn values_mut(&mut self) -> ValuesMut<K, V> {
        self.map.values_mut()
    }

    #[inline(always)]
    pub fn iter(&self) -> Iter<'_, K, V> {
        self.map.iter()
    }

    #[inline(always)]
    pub fn get_mut(&mut self, k: &K) -> Option<&mut V> {
        self.map.get_mut(k)
    }

    #[inline(always)]
    pub fn remove(&mut self, k: &K) -> Option<V> {
        self.map.remove(k)
    }
}

impl<K: RegId + Eq + Hash, V> Index<K> for Registry<K, V> {
    type Output = V;

    #[inline(always)]
    fn index(&self, index: K) -> &Self::Output {
        &self.map[&index]
    }
}

impl<K: RegId + Eq + Hash, V> IndexMut<K> for Registry<K, V> {
    #[inline(always)]
    fn index_mut(&mut self, index: K) -> &mut Self::Output {
        self.map.get_mut(&index).unwrap()
    }
}

pub trait RegId {
    fn new() -> Self;
    fn increment(self) -> Self;
}

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy)]
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

thread_local!(pub static UI_MAP: RefCell<Registry<UiId, Box<dyn Ui>>> = RefCell::new(Registry::new()));

pub trait Drawable {
    fn move_relative(&mut self, distance: Vector2<f64>);
    fn set_pos(&mut self, distance: Vector2<f64>);
    fn draw(&self, rl: &mut RaylibTextureMode<RaylibDrawHandle>, t: &RaylibThread);
    fn get_size(&self) -> Vector2<f64>;
}

pub trait MouseEventHandler {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool;

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>);

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>);

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>);

    fn receive_mouse_wheel(&mut self, mouse_pos: Vector2<f64>, movement: f64) {}

    fn get_on_mouse_enter(&mut self) -> Option<&mut Box<dyn FnMut()>> {
        None
    }

    fn set_on_mouse_enter(&mut self, f: Box<dyn FnMut()>) {}

    fn get_on_mouse_exit(&mut self) -> Option<&mut Box<dyn FnMut()>> {
        None
    }

    fn set_on_mouse_exit(&mut self, f: Box<dyn FnMut()>) {}
}

pub trait Ui: Drawable + MouseEventHandler {}
impl<T: Drawable + MouseEventHandler> Ui for T {}

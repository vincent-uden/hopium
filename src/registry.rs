use core::fmt;
use core::hash::Hash;
use std::collections::hash_map::{Iter, Values, ValuesMut};
use std::collections::HashMap;
use std::ops::{Index, IndexMut};

pub struct Registry<K: RegId + Eq + Hash + Copy + fmt::Debug, V> {
    map: HashMap<K, V>,
    next_id: K,
}

impl<K: RegId + Eq + Hash + Copy + fmt::Debug, V> Registry<K, V> {
    pub fn new() -> Self {
        Self {
            map: HashMap::new(),
            next_id: K::new(),
        }
    }

    pub fn next_id(&self) -> K {
        self.next_id
    }

    pub fn insert(&mut self, v: V) -> K {
        self.map.insert(self.next_id, v);
        let out = self.next_id();
        self.next_id = self.next_id.increment();
        out
    }

    pub fn insert_with_key(&mut self, k: K, v: V) {
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

impl<K: RegId + Eq + Hash + Copy + fmt::Debug, V> Index<K> for Registry<K, V> {
    type Output = V;

    #[inline(always)]
    fn index(&self, index: K) -> &V {
        &self.map[&index]
    }
}

impl<K: RegId + Eq + Hash + Copy + fmt::Debug, V> IndexMut<K> for Registry<K, V> {
    #[inline(always)]
    fn index_mut(&mut self, index: K) -> &mut V {
        self.map.get_mut(&index).unwrap()
    }
}

pub trait RegId {
    fn new() -> Self;
    fn increment(self) -> Self;
}

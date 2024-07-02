use raylib::{RaylibHandle, RaylibThread};

use crate::rendering::renderer::{Renderer, AREA_MAP, BDRY_MAP};

#[derive(Debug)]
pub struct App<'a> {
    renderer: Renderer<'a>,
}

impl<'a> App<'a> {
    pub fn new(
        screen_w: i32,
        screen_h: i32,
        rl: &'a mut RaylibHandle,
        t: &'a RaylibThread,
    ) -> Self {
        App {
            renderer: Renderer::new(screen_w, screen_h, rl, t),
        }
    }

    pub fn update(&mut self) {
        self.renderer.update();
        self.renderer.draw();
    }

    pub fn run(&mut self) {
        while self.renderer.running {
            self.update();
        }
        // These need to be cleared manually before renderer.rl is dropped, otherwise we segfault
        AREA_MAP.with_borrow_mut(|area_map| area_map.clear());
        BDRY_MAP.with_borrow_mut(|bdry_map| bdry_map.clear());
    }
}

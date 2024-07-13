use nalgebra::Vector2;
use raylib::{color::Color, drawing::RaylibDraw};

use crate::{images::ImageId, rendering::renderer::to_raylib, IMAGES, STYLES};

use super::{
    rect::Rect,
    style::{StyleId, StyleType},
    Drawable, MouseEventHandler,
};

pub struct Icon {
    pos: Vector2<f64>,
    pub size: Vector2<f64>,
    pub style: StyleId,
    pub hovered_style: StyleId,
    pub enabled: bool,
    hovered: bool,
    bg: Rect,
    image_id: ImageId,
    pub on_click: Option<Box<dyn Fn()>>,
}

impl Icon {
    pub fn new() -> Self {
        Self {
            pos: Vector2::zeros(),
            size: Vector2::zeros(),
            style: StyleId(StyleType::Icon),
            hovered_style: StyleId(StyleType::IconHovered),
            enabled: true,
            hovered: false,
            bg: Rect::new(),
            image_id: ImageId::IconCoincident,
            on_click: None,
        }
    }

    pub fn set_image(&mut self, image_id: ImageId) {
        IMAGES.with_borrow(|images| {
            let s = &STYLES.read().unwrap()[self.style];
            let img = &images[&image_id];
            self.image_id = image_id;
        });
    }
}

impl Drawable for Icon {
    fn move_relative(&mut self, distance: Vector2<f64>) {
        self.pos += distance;
        self.bg.move_relative(distance);
    }

    fn set_pos(&mut self, pos: Vector2<f64>) {
        self.move_relative(pos - self.pos);
    }

    fn draw(
        &self,
        rl: &mut raylib::prelude::RaylibTextureMode<raylib::prelude::RaylibDrawHandle>,
        t: &raylib::RaylibThread,
    ) {
        let s = &STYLES.read().unwrap()[if self.hovered {
            self.hovered_style
        } else {
            self.style
        }];
        rl.draw_rectangle_v(to_raylib(self.pos), to_raylib(self.size), s.bg_color);
        IMAGES.with_borrow(|images| {
            // Draw image in the center of the icon
            let img = &images[&self.image_id];
            rl.draw_texture(
                img,
                (self.pos.x + (self.size.x - img.width as f64) / 2.0) as i32,
                (self.pos.y + (self.size.y - img.height as f64) / 2.0) as i32,
                Color::new(255, 255, 255, if self.enabled { 255 } else { 128 }),
            );
        });
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
    }
}

impl MouseEventHandler for Icon {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        mouse_pos.x > self.pos.x
            && mouse_pos.x < self.pos.x + self.get_size().x
            && mouse_pos.y > self.pos.y
            && mouse_pos.y < self.pos.y + self.get_size().y
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        self.hovered = self.contains_point(mouse_pos);
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {
        if self.contains_point(mouse_pos) {
            if let Some(on_click) = &self.on_click {
                on_click();
            }
        }
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {}
}

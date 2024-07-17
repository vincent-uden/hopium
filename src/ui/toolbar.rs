use log::debug;
use nalgebra::Vector2;
use raylib::RaylibHandle;

use crate::{combined_draw_handle::CombinedDrawHandle, event::Event, modes::ModeId};

use super::{
    dropdown::DropDown,
    style::{StyleId, StyleType},
    Drawable, MouseEventHandler,
};

pub struct Toolbar {
    pos: Vector2<f64>,
    size: Vector2<f64>,
    style: StyleId,
    hovered_style: StyleId,
    ui_categories: Vec<DropDown>,
    options: Vec<(String, Vec<(String, Event)>)>,
    hovered: bool,
}

impl Toolbar {
    pub fn new() -> Self {
        let options = vec![
            (
                String::from("(Q) Fundamentals"),
                vec![
                    (String::from("(1) Point"), Event::PushMode(ModeId::Point)),
                    (String::from("(2) Line"), Event::PushMode(ModeId::Line)),
                    (String::from("(3) Circle"), Event::PushMode(ModeId::Circle)),
                ],
            ),
            (
                String::from("(W) Rectangles"),
                vec![
                    (
                        String::from("(1) Two point rectangle"),
                        Event::PushMode(ModeId::Point),
                    ),
                    (
                        String::from("(2) Center point rectangle"),
                        Event::PushMode(ModeId::Point),
                    ),
                ],
            ),
        ];
        Self {
            pos: Vector2::new(0.0, 0.0),
            size: Vector2::new(0.0, 0.0),
            style: StyleId(StyleType::Default),
            hovered_style: StyleId(StyleType::Default),
            ui_categories: Vec::new(),
            options,
            hovered: false,
        }
    }

    pub fn update_contents(&mut self, rl: &mut RaylibHandle) {
        self.ui_categories.clear();
        let mut last_pos = Vector2::<f64>::new(0.0, 0.0);

        for (name, opt) in &self.options {
            let names = opt.iter().map(|(n, _)| n.clone()).collect::<Vec<_>>();
            let events = opt.iter().map(|(_, e)| e.clone()).collect::<Vec<_>>();
            let mut category = DropDown::new();
            category.set_contents(name.clone(), names, rl);
            category.option_events = events;
            category.set_pos(last_pos);
            last_pos.x += category.get_size().x;
            self.ui_categories.push(category);
        }
    }
}

impl Drawable for Toolbar {
    fn move_relative(&mut self, distance: Vector2<f64>) {
        self.pos += distance;
        for category in &mut self.ui_categories {
            category.move_relative(distance);
        }
    }

    fn set_pos(&mut self, pos: Vector2<f64>) {
        let diff = pos - self.pos;
        self.move_relative(diff);
    }

    fn draw(&self, rl: &mut CombinedDrawHandle<'_>, t: &raylib::RaylibThread) {
        for category in &self.ui_categories {
            category.draw(rl, t);
        }
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
    }
}

impl MouseEventHandler for Toolbar {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        mouse_pos.x > self.pos.x
            && mouse_pos.x < self.pos.x + self.get_size().x
            && mouse_pos.y > self.pos.y
            && mouse_pos.y < self.pos.y + self.get_size().y
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        self.hovered = self.contains_point(mouse_pos);
        for category in &mut self.ui_categories {
            category.receive_mouse_pos(mouse_pos);
        }
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {
        for category in &mut self.ui_categories {
            category.receive_mouse_down(mouse_pos, press);
        }
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {
        for category in &mut self.ui_categories {
            category.receive_mouse_up(mouse_pos, press);
        }
    }
}

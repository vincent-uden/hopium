use nalgebra::Vector2;

use crate::{
    cad::entity::{BiConstraint, ConstraintType},
    combined_draw_handle::CombinedDrawHandle,
    event::Event,
    images::ImageId,
    APP_STATE, EVENT_QUEUE,
};

use super::{
    icon::Icon,
    style::{StyleId, StyleType},
    Drawable, MouseEventHandler,
};

pub struct ConstraintSelector {
    pos: Vector2<f64>,
    size: Vector2<f64>,
    style: StyleId,
    hovered_style: StyleId,
    constraint_types: Vec<ConstraintType>,
    icons: Vec<Icon>,
    hovered: bool,
}

impl ConstraintSelector {
    pub fn new() -> Self {
        let mut icons = vec![];
        let icon_ids = [
            ImageId::IconAngle,
            ImageId::IconCoincident,
            ImageId::IconColinear,
            ImageId::IconDistance,
            ImageId::IconHorizontal,
            ImageId::IconParallel,
            ImageId::IconPerpendicular,
            ImageId::IconTangent,
            ImageId::IconVertical,
        ];
        let constraint_types = vec![
            ConstraintType::Angle { x: 0.0 },
            ConstraintType::Coincident,
            ConstraintType::Colinear,
            ConstraintType::Distance { x: 0.0 },
            ConstraintType::Horizontal,
            ConstraintType::Parallel,
            ConstraintType::Perpendicular,
            ConstraintType::Tangent,
            ConstraintType::Vertical,
        ];
        for (i, icon_id) in icon_ids.iter().enumerate() {
            let mut icon = Icon::new();
            icon.set_image(*icon_id);
            icon.size = Vector2::new(40.0, 40.0);
            icon.set_pos(Vector2::new(i as f64 * icon.get_size().x, 0.0));
            let c = constraint_types[i];
            icon.on_click = Some(Box::new(move || {
                let mut eq = EVENT_QUEUE.lock().unwrap();
                eq.post_event(Event::Constrain { constraint_type: c });
            }));
            icons.push(icon);
        }
        Self {
            pos: Vector2::new(0.0, 0.0),
            size: Vector2::new(
                icons.len() as f64 * icons[0].get_size().x,
                icons[0].get_size().y,
            ),
            style: StyleId(StyleType::Default),
            hovered_style: StyleId(StyleType::Default),
            icons,
            constraint_types,
            hovered: false,
        }
    }
}

impl Drawable for ConstraintSelector {
    fn move_relative(&mut self, distance: Vector2<f64>) {
        self.pos += distance;
        for icon in &mut self.icons {
            icon.move_relative(distance);
        }
    }

    fn set_pos(&mut self, pos: Vector2<f64>) {
        let diff = pos - self.pos;
        self.move_relative(diff);
    }

    fn draw(&self, rl: &mut CombinedDrawHandle<'_>, t: &raylib::RaylibThread) {
        for (icon, constraint_type) in self.icons.iter().zip(&self.constraint_types) {
            icon.draw(rl, t);
        }
    }

    fn get_size(&self) -> Vector2<f64> {
        self.size
    }
}

impl MouseEventHandler for ConstraintSelector {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        mouse_pos.x > self.pos.x
            && mouse_pos.x < self.pos.x + self.get_size().x
            && mouse_pos.y > self.pos.y
            && mouse_pos.y < self.pos.y + self.get_size().y
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        self.hovered = self.contains_point(mouse_pos);
        for icon in &mut self.icons {
            icon.receive_mouse_pos(mouse_pos);
        }
        for (icon, constraint_type) in self.icons.iter_mut().zip(&self.constraint_types) {
            let state = APP_STATE.lock().unwrap();
            icon.enabled = false;
            if state.selected.len() == 2 {
                let e1 = &state.sketch.fundamental_entities[state.selected[0]];
                let e2 = &state.sketch.fundamental_entities[state.selected[1]];
                if BiConstraint::possible(e1, e2, constraint_type) {
                    icon.enabled = true;
                }
            }
        }
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {
        for icon in &mut self.icons {
            if icon.contains_point(mouse_pos) {
                icon.receive_mouse_down(mouse_pos, press);
            }
        }
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &crate::modes::MousePress) {}
}

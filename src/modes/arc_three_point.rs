use crate::{
    cad::entity::{
        BiConstraint, Circle, ConstraintType, FundamentalEntity, GuidedEntity, Line, Point,
    },
    event::Event,
    APP_STATE, EVENT_QUEUE,
};

use nalgebra::{Normed, Vector2};
use raylib::ffi::{KeyboardKey, MouseButton};

use super::{Mode, ModeId, MousePress};

#[derive(Debug)]
pub struct ArcThreePoint {}

impl ArcThreePoint {
    pub fn new() -> Self {
        Self {}
    }

    fn sketch_click(&self, click_pos: Vector2<f64>, select_radius: f64, press: MousePress) {
        let mut state = APP_STATE.lock().unwrap();
        if press.button == raylib::ffi::MouseButton::MOUSE_BUTTON_LEFT {
            state.pending_clicks.push(click_pos);
        }
        if press.button == raylib::ffi::MouseButton::MOUSE_BUTTON_RIGHT {
            state.pending_clicks.clear();
        }

        if state.pending_clicks.len() == 3 {
            let mut p1 = state.pending_clicks[0];
            let mut p2 = state.pending_clicks[1];
            let mut p3 = state.pending_clicks[2];
            state.pending_clicks.clear();
            // TODO: Constrain these together
            let start_id = state.sketch.query_or_insert_point(&p1, select_radius);
            let end_id = state.sketch.query_or_insert_point(&p2, select_radius);
            let middle_id = state.sketch.query_or_insert_point(&p3, select_radius);

            let entity_reg = &mut state.sketch.fundamental_entities;
            if let FundamentalEntity::Point(p) = entity_reg[start_id] {
                p1 = p.pos;
            }
            if let FundamentalEntity::Point(p) = entity_reg[end_id] {
                p2 = p.pos;
            }
            if let FundamentalEntity::Point(p) = entity_reg[middle_id] {
                p3 = p.pos;
            }

            let circle_id = entity_reg
                .insert(FundamentalEntity::circle_from_three_coords(&p1, &p2, &p3).unwrap());

            let guided_entity_reg = &mut state.sketch.guided_entities;
            guided_entity_reg.insert(GuidedEntity::ArcThreePoint {
                start: start_id,
                middle: middle_id,
                end: end_id,
                circle: circle_id,
            });

            state.sketch.bi_constraints.push(BiConstraint::new(
                start_id,
                circle_id,
                ConstraintType::Coincident,
            ));
            state.sketch.bi_constraints.push(BiConstraint::new(
                middle_id,
                circle_id,
                ConstraintType::Coincident,
            ));
            state.sketch.bi_constraints.push(BiConstraint::new(
                end_id,
                circle_id,
                ConstraintType::Coincident,
            ));
        }
    }
}

impl Mode for ArcThreePoint {
    fn id(&self) -> ModeId {
        ModeId::Line
    }

    fn process_event(&self, event: crate::event::Event) -> bool {
        match event {
            Event::SketchClick {
                pos,
                sketch_space_select_radius,
                press,
            } => {
                self.sketch_click(pos, sketch_space_select_radius, press);
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

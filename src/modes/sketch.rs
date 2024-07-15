use nalgebra::Vector2;
use raylib::{
    ffi::{KeyboardKey, MouseButton},
    RaylibHandle,
};

use crate::{
    cad::entity::BiConstraint, event::Event, ui::sketchviewer::SketchViewer, APP_STATE, EVENT_QUEUE,
};

use super::{KeyPress, Mode, ModeId, MousePress};

#[derive(Debug)]
pub struct SketchMode {}

impl SketchMode {
    pub fn new() -> Self {
        Self {}
    }

    fn sketch_click(&self, click_pos: Vector2<f64>, select_radius: f64, press: MousePress) {
        let mut state = APP_STATE.lock().unwrap();
        let mut closest = None;
        let mut closest_id = None;
        let mut closest_dist = f64::INFINITY;

        for (id, e) in state.sketch.fundamental_entities.iter() {
            let dist = e.distance_to_position(&click_pos);
            if dist <= select_radius
                && ((dist < closest_dist && SketchViewer::can_ovveride_selection(e, &closest))
                    || SketchViewer::should_ovveride_selection(e, &closest))
            {
                closest = Some(*e);
                closest_id = Some(*id);
                closest_dist = dist;
            }
        }

        if !press.shift {
            state.selected.clear();
        }
        if let Some(id) = closest_id {
            state.selected.push(id);
        }
    }

    fn constrain(&self, constraint_type: crate::cad::entity::ConstraintType) {
        let mut state = APP_STATE.lock().unwrap();
        if state.selected.len() == 2 {
            let id1 = state.selected[0];
            let id2 = state.selected[1];
            let e1 = &state.sketch.fundamental_entities[id1];
            let e2 = &state.sketch.fundamental_entities[id2];
            if BiConstraint::possible(e1, e2, &constraint_type) {
                let bi_constraint = BiConstraint::new(id1, id2, constraint_type);
                state.sketch.bi_constraints.push(bi_constraint);
                state.selected.clear();
            }
        }
    }
}

impl Mode for SketchMode {
    fn id(&self) -> super::ModeId {
        ModeId::Sketch
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
            Event::Constrain { constraint_type } => {
                self.constrain(constraint_type);
                true
            }
            _ => false,
        }
    }

    fn key_press(&mut self, key: &KeyPress, rl: &mut RaylibHandle) -> bool {
        let mut state = APP_STATE.lock().unwrap();
        let mut eq = EVENT_QUEUE.lock().unwrap();
        let mut out = true;
        match key.key {
            KeyboardKey::KEY_ENTER => {
                state.solving = true;
            }
            // These keycodes are weird due to nordic layout. The intention is to increase zoom
            // with (Ctrl +) and decrease it with (Ctrl -)
            KeyboardKey::KEY_SLASH => {
                if key.ctrl {
                    eq.post_event(Event::DecreaseZoom);
                }
            }
            KeyboardKey::KEY_MINUS => {
                if key.ctrl {
                    eq.post_event(Event::IncreaseZoom);
                }
            }
            _ => {
                out = false;
            }
        }
        out
    }

    fn key_release(&mut self, key: &KeyPress) -> bool {
        let mut state = APP_STATE.lock().unwrap();
        let mut out = true;
        match key.key {
            KeyboardKey::KEY_ENTER => {
                state.solving = false;
            }
            _ => {
                out = false;
            }
        }
        out
    }
}

use log::debug;
use nalgebra::Vector2;
use raylib::{
    ffi::{KeyboardKey, MouseButton},
    RaylibHandle,
};

use crate::{
    cad::entity::{BiConstraint, ConstraintType},
    event::Event,
    ui::sketchviewer::SketchViewer,
    APP_STATE, EVENT_QUEUE,
};

use super::{data_entry::Form, KeyPress, Mode, ModeId, MousePress};

#[derive(Debug)]
pub struct SketchMode {
    pending_constraint: Option<BiConstraint>,
}

impl SketchMode {
    pub fn new() -> Self {
        Self {
            pending_constraint: None,
        }
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
                let mut inside = true;
                for (guided_id, guided) in state.sketch.guided_entities.iter() {
                    if guided.refers_to(*id) {
                        inside = guided.filter_selection_attempt(&state, click_pos);
                        break;
                    }
                }
                if inside {
                    closest = Some(*e);
                    closest_id = Some(*id);
                    closest_dist = dist;
                }
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
        let mut eq = EVENT_QUEUE.lock().unwrap();
        if state.selected.len() == 2 {
            let id1 = state.selected[0];
            let id2 = state.selected[1];
            let e1 = &state.sketch.fundamental_entities[id1];
            let e2 = &state.sketch.fundamental_entities[id2];
            if BiConstraint::possible(e1, e2, &constraint_type) {
                let bi_constraint = BiConstraint::new(id1, id2, constraint_type);
                match &constraint_type {
                    ConstraintType::Angle { x } => {
                        state.form = Some(Form::angle());
                        state.form_focused = 0;
                        eq.post_event(Event::PushMode(ModeId::DataEntry));
                        state.pending_constraint = Some(bi_constraint);
                    }
                    ConstraintType::Distance { x } => {
                        state.form = Some(Form::distance());
                        state.form_focused = 0;
                        eq.post_event(Event::PushMode(ModeId::DataEntry));
                        state.pending_constraint = Some(bi_constraint);
                    }
                    _ => {
                        state.sketch.bi_constraints.push(bi_constraint);
                        state.selected.clear();
                    }
                }
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
            Event::ConfirmDataEntry => {
                let mut state = APP_STATE.lock().unwrap();
                if let Some(mut constraint) = state.pending_constraint.take() {
                    match constraint.c {
                        ConstraintType::Angle { x } => {
                            let angle = state.form.as_ref().unwrap().entries[0]
                                .buffer
                                .parse()
                                .unwrap();
                            constraint.c = ConstraintType::Angle { x: angle };
                        }
                        ConstraintType::Distance { x } => {
                            let distance = state.form.as_ref().unwrap().entries[0]
                                .buffer
                                .parse()
                                .unwrap();
                            constraint.c = ConstraintType::Distance { x: distance };
                        }
                        _ => {}
                    }
                    state.sketch.bi_constraints.push(constraint);
                    state.selected.clear();
                    state.form = None;
                    let mut eq = EVENT_QUEUE.lock().unwrap();
                    eq.post_event(Event::PopMode);
                }
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
            KeyboardKey::KEY_ESCAPE => {
                eq.post_event(Event::PopMode);
            }
            KeyboardKey::KEY_ENTER => {
                state.solving = !state.solving;
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
            KeyboardKey::KEY_P => {
                if key.vanilla() {
                    eq.post_event(Event::PushMode(ModeId::Point));
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_L => {
                if key.vanilla() {
                    eq.post_event(Event::PushMode(ModeId::Line));
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_C => {
                if key.vanilla() {
                    eq.post_event(Event::PushMode(ModeId::Circle));
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_Q => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarOpenCategory { idx: 0 });
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_W => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarOpenCategory { idx: 1 });
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_ONE => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarConfirm { idx: 0 });
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_TWO => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarConfirm { idx: 1 });
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_THREE => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarConfirm { idx: 2 });
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_FOUR => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarConfirm { idx: 3 });
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_FIVE => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarConfirm { idx: 4 });
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_SIX => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarConfirm { idx: 5 });
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_SEVEN => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarConfirm { idx: 6 });
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_EIGHT => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarConfirm { idx: 7 });
                } else {
                    out = false;
                }
            }
            KeyboardKey::KEY_NINE => {
                if key.vanilla() {
                    eq.post_event(Event::ToolbarConfirm { idx: 8 });
                } else {
                    out = false;
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
            _ => {
                out = false;
            }
        }
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

use nalgebra::Vector2;

use crate::{event::Event, ui::sketchviewer::SketchViewer, APP_STATE};

use super::{Mode, ModeId};

#[derive(Debug)]
pub struct SketchMode {}

impl SketchMode {
    pub fn new() -> Self {
        Self {}
    }

    fn sketch_click(&self, click_pos: Vector2<f64>, select_radius: f64) {
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

        if let Some(id) = closest_id {
            // Duplication for now, will be needed later when parsing Shift
            state.selected.clear();
            state.selected.push(id);
        } else {
            state.selected.clear();
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
            } => {
                self.sketch_click(pos, sketch_space_select_radius);
                true
            }
            _ => false,
        }
    }
}

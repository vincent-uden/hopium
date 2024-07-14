use nalgebra::Vector2;

use crate::{
    cad::entity::{FundamentalEntity, Point},
    event::Event,
    APP_STATE,
};

use super::{Mode, ModeId, MousePress};

#[derive(Debug)]
pub struct PointMode {}

impl PointMode {
    pub fn new() -> Self {
        Self {}
    }

    fn sketch_click(&self, click_pos: Vector2<f64>, press: MousePress) {
        let mut state = APP_STATE.lock().unwrap();
        if press.button == raylib::ffi::MouseButton::MOUSE_BUTTON_LEFT {
            state
                .sketch
                .fundamental_entities
                .insert(FundamentalEntity::Point(Point { pos: click_pos }));
        }
    }
}

impl Mode for PointMode {
    fn id(&self) -> ModeId {
        ModeId::Point
    }

    fn process_event(&self, event: crate::event::Event) -> bool {
        match event {
            Event::SketchClick {
                pos,
                sketch_space_select_radius,
                press,
            } => {
                self.sketch_click(pos, press);
                true
            }
            _ => false,
        }
    }
}

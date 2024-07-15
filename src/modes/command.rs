use nalgebra::Vector2;

use crate::{
    cad::entity::{FundamentalEntity, Point},
    event::Event,
    APP_STATE,
};

use super::{Mode, ModeId, MousePress};

#[derive(Debug)]
pub struct CommandMode {}

impl CommandMode {
    pub fn new() -> Self {
        Self {}
    }

    fn sketch_click(&self, click_pos: Vector2<f64>, press: MousePress) {}
}

impl Mode for CommandMode {
    fn id(&self) -> ModeId {
        ModeId::Command
    }

    fn process_event(&self, event: crate::event::Event) -> bool {
        false
    }
}

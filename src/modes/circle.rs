use super::{Mode, ModeId};

#[derive(Debug)]
pub struct CircleMode {}

impl CircleMode {
    pub fn new() -> Self {
        Self {}
    }
}

impl Mode for CircleMode {
    fn id(&self) -> ModeId {
        ModeId::Circle
    }

    fn process_event(&self, event: crate::event::Event) -> bool {
        false
    }
}

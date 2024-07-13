use super::{Mode, ModeId};

#[derive(Debug)]
pub struct LineMode {}

impl LineMode {
    pub fn new() -> Self {
        Self {}
    }
}

impl Mode for LineMode {
    fn id(&self) -> ModeId {
        ModeId::Line
    }

    fn process_event(&self, event: crate::event::Event) -> bool {
        false
    }
}

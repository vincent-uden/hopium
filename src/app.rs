#[derive(Debug)]
pub struct App {
    pub running: bool,
}

impl App {
    pub fn new() -> Self {
        App { running: true }
    }
}

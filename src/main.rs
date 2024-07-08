use std::sync::Mutex;

use app::{App, State};
use event::EventQueue;
use lazy_static::lazy_static;
use modes::ModeStack;

mod app;
#[allow(dead_code, unused_variables)]
mod event;
#[allow(dead_code, unused_variables)]
mod modes;
#[allow(dead_code, unused_variables)]
mod rendering;

lazy_static! {
    static ref app_state: Mutex<State> = Mutex::new(State::new());
}
lazy_static! {
    static ref event_queue: Mutex<EventQueue> = Mutex::new(EventQueue::new());
}
lazy_static! {
    static ref mode_stack: Mutex<ModeStack> = Mutex::new(ModeStack::new());
}

fn main() {
    let (mut rl, thread) = raylib::init()
        .size(1600, 900)
        .undecorated()
        .msaa_4x()
        .vsync()
        .build();

    let mut app = App::new(1600, 900, &mut rl, &thread);
    app.run();
}

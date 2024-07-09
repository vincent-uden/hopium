use std::sync::{Mutex, RwLock};

use app::{App, State};
use event::EventQueue;
use lazy_static::lazy_static;
use modes::ModeStack;
use ui::style::Style;

mod app;
#[allow(dead_code, unused_variables)]
mod event;
#[allow(dead_code, unused_variables)]
mod modes;
#[allow(dead_code, unused_variables)]
mod registry;
#[allow(dead_code, unused_variables)]
mod rendering;
#[allow(dead_code, unused_variables)]
mod ui;

lazy_static! {
    static ref APP_STATE: Mutex<State> = Mutex::new(State::new());
}
lazy_static! {
    static ref EVENT_QUEUE: Mutex<EventQueue> = Mutex::new(EventQueue::new());
}
lazy_static! {
    static ref MODE_STACK: Mutex<ModeStack> = Mutex::new(ModeStack::new());
}
lazy_static! {
    static ref STYLE: RwLock<Style> = RwLock::new(Style::default());
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

#![feature(map_many_mut)]

use std::{
    cell::RefCell,
    collections::HashMap,
    fs,
    path::PathBuf,
    sync::{Mutex, RwLock},
};

use app::{App, State};
use clap::Parser;
use event::EventQueue;
use images::{populate_images, ImageId};
use lazy_static::lazy_static;
use log::LevelFilter;
use modes::ModeStack;
use raylib::texture::Texture2D;
use registry::Registry;
use simplelog::{ColorChoice, CombinedLogger, Config, TermLogger, TerminalMode};
use ui::style::{populate_styles, Style, StyleId};

mod app;
#[allow(dead_code, unused_variables)]
mod cad;
#[allow(dead_code, unused_variables)]
mod combined_draw_handle;
#[allow(dead_code, unused_variables)]
mod event;
#[allow(dead_code, unused_variables)]
mod images;
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
    static ref STYLES: RwLock<Registry<StyleId, Style>> = RwLock::new(Registry::new());
}
thread_local! {
    pub static IMAGES: RefCell<HashMap<ImageId, Texture2D>> = RefCell::new(HashMap::new());
}

#[derive(Parser)]
pub struct Args {
    #[arg(short, long)]
    pub file: Option<PathBuf>,
}

fn main() {
    let args = Args::parse();

    CombinedLogger::init(vec![TermLogger::new(
        LevelFilter::Debug,
        Config::default(),
        TerminalMode::Mixed,
        ColorChoice::Auto,
    )])
    .unwrap();
    let (mut rl, thread) = raylib::init()
        .size(1600, 900)
        .undecorated()
        .msaa_4x()
        .vsync()
        .build();

    populate_styles();
    populate_images(&mut rl, &thread);

    let mut app = App::new(1600, 900, &mut rl, &thread);
    if let Ok(contents) = fs::read_to_string("layout.json") {
        let mut eq: EventQueue = serde_json::from_str(&contents).unwrap();
        eq.reset_history_index();
        let mut event_queue = EVENT_QUEUE.lock().unwrap();
        while let Some(e) = eq.get_next_history_event() {
            app.process_event(e);
        }
        *event_queue = eq.clone();
    }
    {
        let mut state = APP_STATE.lock().unwrap();
        if let Some(file) = args.file {
            state.sketch = serde_json::from_str(&fs::read_to_string(file).unwrap()).unwrap();
        }
    }
    app.run();
}

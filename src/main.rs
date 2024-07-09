use std::sync::{Mutex, RwLock};

use app::{App, State};
use event::EventQueue;
use lazy_static::lazy_static;
use modes::ModeStack;
use raylib::color::Color;
use registry::Registry;
use ui::style::{Style, StyleId};

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
    static ref STYLES: RwLock<Registry<StyleId, Style>> = RwLock::new(Registry::new());
}

fn main() {
    {
        let mut styles = STYLES.write().unwrap();
        styles.insert_with_key(StyleId(ui::style::StyleType::Default), Style::default());
        styles.insert_with_key(
            StyleId(ui::style::StyleType::Area),
            Style {
                bg_color: Color::new(51, 51, 51, 255),
                ..Default::default()
            },
        );
        styles.insert_with_key(
            StyleId(ui::style::StyleType::AreaText),
            Style {
                color: Color::new(15, 15, 15, 255),
                hovered_color: Color::new(185, 15, 15, 255),
                ..Default::default()
            },
        );
        styles.insert_with_key(
            StyleId(ui::style::StyleType::Boundary),
            Style {
                bg_color: Color::new(15, 15, 15, 255),
                ..Default::default()
            },
        );
    }
    let (mut rl, thread) = raylib::init()
        .size(1600, 900)
        .undecorated()
        .msaa_4x()
        .vsync()
        .build();

    let mut app = App::new(1600, 900, &mut rl, &thread);
    app.run();
}

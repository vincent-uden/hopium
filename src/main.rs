use app::App;
use raylib::{color::Color, drawing::RaylibDraw};

mod app;
mod rendering;

fn main() {
    let (mut rl, thread) = raylib::init()
        .size(1600, 900)
        .undecorated()
        .msaa_4x()
        .vsync()
        .build();

    let mut app = App::new();

    while !rl.window_should_close() && app.running {
        let mut d = rl.begin_drawing(&thread);

        d.clear_background(Color::GRAY);
        d.draw_fps(10, 10);
    }
}

use app::App;
use raylib::{color::Color, drawing::RaylibDraw};

mod app;
#[allow(dead_code, unused_variables)]
mod rendering;

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

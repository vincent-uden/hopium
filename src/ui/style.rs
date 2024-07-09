use raylib::color::Color;

#[derive(Debug)]
pub struct Style {
    pub bg_color: Color,
    pub border_color: Color,

    pub text_2xl: f32,
}

impl Default for Style {
    fn default() -> Self {
        Self {
            bg_color: Color::new(51, 51, 51, 255),
            border_color: Color::new(15, 15, 15, 255),
            text_2xl: 40.0,
        }
    }
}

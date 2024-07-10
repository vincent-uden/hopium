use raylib::color::Color;

use crate::registry::RegId;

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy)]
pub enum StyleType {
    Default,
    Area,
    AreaText,
    Boundary,
    DropDown,
    DropDownHovered,
}

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy)]
pub struct StyleId(pub StyleType);

impl RegId for StyleId {
    fn new() -> Self {
        Self(StyleType::Default)
    }

    fn increment(self) -> Self {
        self
    }
}

#[derive(Debug)]
pub struct Style {
    pub bg_color: Color,
    pub color: Color,
    pub hovered_color: Color,
    pub font_size: f32,
}

impl Default for Style {
    fn default() -> Self {
        Self {
            bg_color: Color::new(51, 51, 51, 255),
            color: Color::new(15, 15, 15, 255),
            hovered_color: Color::new(15, 15, 15, 255),
            font_size: 40.0,
        }
    }
}

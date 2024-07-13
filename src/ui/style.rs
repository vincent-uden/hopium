use colored::Colorize;
use raylib::color::Color;
use strum::IntoEnumIterator;
use strum_macros::EnumIter;

use crate::{registry::RegId, STYLES};

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy, EnumIter)]
pub enum StyleType {
    Default,
    Area,
    AreaText,
    Boundary,
    DropDown,
    DropDownHovered,
    Icon,
    IconHovered,
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

pub fn populate_styles() {
    {
        let mut styles = STYLES.write().unwrap();
        styles.insert_with_key(StyleId(StyleType::Default), Style::default());
        styles.insert_with_key(
            StyleId(StyleType::Area),
            Style {
                bg_color: Color::new(51, 51, 51, 255),
                ..Default::default()
            },
        );
        styles.insert_with_key(
            StyleId(StyleType::AreaText),
            Style {
                color: Color::new(15, 15, 15, 255),
                hovered_color: Color::new(185, 15, 15, 255),
                ..Default::default()
            },
        );
        styles.insert_with_key(
            StyleId(StyleType::Boundary),
            Style {
                bg_color: Color::new(15, 15, 15, 255),
                ..Default::default()
            },
        );
        styles.insert_with_key(
            StyleId(StyleType::DropDown),
            Style {
                color: Color::new(200, 200, 200, 255),
                bg_color: Color::new(31, 31, 31, 255),
                ..Default::default()
            },
        );
        styles.insert_with_key(
            StyleId(StyleType::DropDownHovered),
            Style {
                color: Color::new(200, 200, 200, 255),
                bg_color: Color::new(71, 71, 71, 255),
                ..Default::default()
            },
        );
        styles.insert_with_key(
            StyleId(StyleType::Icon),
            Style {
                ..Default::default()
            },
        );
        styles.insert_with_key(
            StyleId(StyleType::IconHovered),
            Style {
                bg_color: Color::new(71, 71, 71, 255),
                ..Default::default()
            },
        );

        let missing = StyleType::iter()
            .filter(|k| !styles.get_mut(&StyleId(*k)).is_some())
            .collect::<Vec<_>>();
        for k in &missing {
            println!(
                "{}",
                format!("Missing style {:?}", k).to_uppercase().red().bold()
            )
        }
        if missing.len() > 0 {
            let msg = "Missing styles".to_uppercase().red().bold();
            panic!("{}", msg);
        }
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

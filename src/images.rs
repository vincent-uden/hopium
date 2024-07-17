use colored::Colorize;
use raylib::{RaylibHandle, RaylibThread};
use serde::{Deserialize, Serialize};
use strum::IntoEnumIterator;
use strum_macros::EnumIter;

use crate::IMAGES;

#[allow(clippy::enum_variant_names)]
#[derive(
    Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy, Serialize, Deserialize, EnumIter,
)]
pub enum ImageId {
    IconAngle,
    IconCoincident,
    IconColinear,
    IconDistance,
    IconHorizontal,
    IconParallel,
    IconPerpendicular,
    IconTangent,
    IconVertical,
}

pub fn populate_images(rl: &mut RaylibHandle, t: &RaylibThread) {
    IMAGES.with_borrow_mut(|images| {
        images.insert(
            ImageId::IconAngle,
            rl.load_texture(t, "./assets/icons/Angle.png").unwrap(),
        );
        images.insert(
            ImageId::IconCoincident,
            rl.load_texture(t, "./assets/icons/Coincident.png").unwrap(),
        );
        images.insert(
            ImageId::IconColinear,
            rl.load_texture(t, "./assets/icons/Colinear.png").unwrap(),
        );
        images.insert(
            ImageId::IconDistance,
            rl.load_texture(t, "./assets/icons/Distance.png").unwrap(),
        );
        images.insert(
            ImageId::IconHorizontal,
            rl.load_texture(t, "./assets/icons/Horizontal.png").unwrap(),
        );
        images.insert(
            ImageId::IconParallel,
            rl.load_texture(t, "./assets/icons/Parallel.png").unwrap(),
        );
        images.insert(
            ImageId::IconPerpendicular,
            rl.load_texture(t, "./assets/icons/Perpendicular.png")
                .unwrap(),
        );
        images.insert(
            ImageId::IconTangent,
            rl.load_texture(t, "./assets/icons/Tangent.png").unwrap(),
        );
        images.insert(
            ImageId::IconVertical,
            rl.load_texture(t, "./assets/icons/Vertical.png").unwrap(),
        );

        let missing = ImageId::iter()
            .filter(|k| !images.contains_key(k))
            .collect::<Vec<_>>();
        for k in &missing {
            println!(
                "{}",
                format!("Missing image {:?}", k).to_uppercase().red().bold()
            )
        }
        if !missing.is_empty() {
            let msg = "Missing images".to_uppercase().red().bold();
            panic!("{}", msg);
        }
    });
}

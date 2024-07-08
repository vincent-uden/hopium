use std::collections::HashMap;

use nalgebra::Vector2;
use raylib::{
    color::Color,
    drawing::{RaylibDraw, RaylibDrawHandle, RaylibTextureModeExt},
    math::Rectangle,
    texture::{RaylibTexture2D, RenderTexture2D},
    RaylibThread,
};
use serde::{Deserialize, Serialize};

use super::{
    boundary::{Boundary, BoundaryId, BoundaryOrientation},
    renderer::{to_raylib, MouseEventHandler, BDRY_MAP},
};

#[derive(Debug, Deserialize, Serialize)]
pub enum RenderAnchor {
    Left,
    Center,
    Right,
}

#[derive(Debug, Deserialize, Serialize, Clone, Copy)]
pub enum AreaType {
    Empty,
    Viewport3d,
    ToolSelection,
    ContraintSelection,
    SketchViewer,
}

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy)]
pub struct AreaId(pub i64);

impl AreaId {
    pub fn increment(self) -> Self {
        let AreaId(id) = self;
        Self(id + 1)
    }
}

impl Default for AreaId {
    fn default() -> Self {
        AreaId(-1)
    }
}

#[derive(Debug)]
pub struct Area {
    pub id: AreaId,
    pub area_type: AreaType,
    pub screen_rect: Rectangle,
    pub screen_pos: Vector2<f64>,
    pub active: bool,
    pub anchor: RenderAnchor,
    pub texture: RenderTexture2D,
}

impl Area {
    pub fn new(
        area_type: AreaType,
        id: AreaId,
        screen_rect: Rectangle,
        screen_pos: Vector2<f64>,
        texture: RenderTexture2D,
    ) -> Self {
        let mut out = Self {
            id,
            area_type,
            screen_rect,
            screen_pos,
            active: false,
            anchor: RenderAnchor::Left,
            texture,
        };
        match area_type {
            AreaType::Viewport3d => {
                out.build_viewport_3d();
            }
            AreaType::ToolSelection => {
                out.build_tool_selection();
            }
            AreaType::ContraintSelection => {
                out.build_constraint_selection();
            }
            AreaType::SketchViewer => {
                out.build_sketch_viewer();
            }
            _ => {}
        }
        out
    }

    pub fn draw(&mut self, d: &mut RaylibDrawHandle, t: &RaylibThread) {
        {
            let mut td = d.begin_texture_mode(t, &mut self.texture);
            td.clear_background(Color::GRAY);
            // TODO: Translate and draw contained ui when it exists
        }
        let mut draw_rect = self.screen_rect;
        draw_rect.y = -draw_rect.height;
        draw_rect.height *= -1.0;
        match self.anchor {
            RenderAnchor::Left => {}
            RenderAnchor::Center => {
                draw_rect.x = (self.texture.width() as f32 - self.screen_rect.width) / 2.0;
                draw_rect.y = (self.texture.height() as f32 - self.screen_rect.height) / 2.0;
            }
            RenderAnchor::Right => {
                draw_rect.x = self.texture.width() as f32 - self.screen_rect.width;
            }
        }
        draw_rect.x = draw_rect.x.round();
        draw_rect.y = draw_rect.y.round();
        draw_rect.width = draw_rect.width.round();
        draw_rect.height = draw_rect.height.round();
        d.draw_texture_rec(
            &self.texture,
            draw_rect,
            to_raylib(self.screen_pos),
            Color::WHITE,
        );
    }

    pub fn delete_this_from_boundaries(&mut self) {
        BDRY_MAP.with_borrow_mut(|bdry_map| {
            for bdry in bdry_map.values_mut() {
                bdry.delete_area(&self.id);
            }
        })
    }

    pub fn is_left_of(&self, bdry_id: &BoundaryId) -> bool {
        let mut is_true = true;
        BDRY_MAP.with_borrow(|bdry_map| {
            let bdry = &bdry_map[bdry_id];
            is_true =
                bdry.orientation == BoundaryOrientation::Vertical && bdry.side1.contains(&self.id);
        });
        is_true
    }

    pub fn is_right_of(&self, bdry_id: &BoundaryId) -> bool {
        let mut is_true = true;
        BDRY_MAP.with_borrow(|bdry_map| {
            let bdry = &bdry_map[bdry_id];
            is_true =
                bdry.orientation == BoundaryOrientation::Vertical && bdry.side2.contains(&self.id);
        });
        is_true
    }

    pub fn is_below(&self, bdry_id: &BoundaryId) -> bool {
        let mut is_true = true;
        BDRY_MAP.with_borrow(|bdry_map| {
            let bdry = &bdry_map[bdry_id];
            is_true =
                bdry.orientation == BoundaryOrientation::Vertical && bdry.side2.contains(&self.id);
            bdry.orientation == BoundaryOrientation::Horizontal && bdry.side2.contains(&self.id)
        });
        is_true
    }

    pub fn is_above(&self, bdry_id: &BoundaryId) -> bool {
        let mut is_true = true;
        BDRY_MAP.with_borrow(|bdry_map| {
            let bdry = &bdry_map[bdry_id];
            is_true =
                bdry.orientation == BoundaryOrientation::Horizontal && bdry.side1.contains(&self.id)
        });
        is_true
    }

    pub fn further_down_bdry_tree(
        &self,
        bdry_map: &HashMap<BoundaryId, Boundary>,
    ) -> Vec<BoundaryId> {
        let mut out = vec![];
        for (id, bdry) in bdry_map.iter() {
            if bdry.side1.contains(&self.id) {
                out.push(*id);
            }
        }
        out
    }

    fn build_viewport_3d(&mut self) {
        todo!()
    }
    fn build_tool_selection(&mut self) {
        todo!()
    }
    fn build_constraint_selection(&mut self) {
        todo!()
    }
    fn build_sketch_viewer(&mut self) {
        todo!()
    }
}

impl MouseEventHandler for Area {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        if mouse_pos.x > self.screen_pos.x
            && mouse_pos.x < self.screen_pos.x + self.screen_rect.width as f64
            && mouse_pos.y > self.screen_pos.y
            && mouse_pos.y < self.screen_pos.y + self.screen_rect.height as f64
        {
            return true;
        }
        false
    }

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    fn receive_mouse_wheel(&mut self, mouse_pos: Vector2<f64>, movement: f64) {
        todo!()
    }
}

impl PartialEq for Area {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}
use core::fmt;
use std::collections::HashMap;

use nalgebra::Vector2;
use raylib::{
    color::Color,
    drawing::{RaylibDraw, RaylibDrawHandle, RaylibTextureModeExt},
    math::Rectangle,
    texture::{RaylibTexture2D, RenderTexture2D},
    RaylibHandle, RaylibThread,
};
use serde::{Deserialize, Serialize};

use crate::{
    ui::{
        self, text::TextAlignment, Drawable, MouseEventHandler, RegId, Registry, Ui, UiId, UI_MAP,
    },
    STYLE,
};

use super::{
    boundary::{Boundary, BoundaryId, BoundaryOrientation},
    renderer::{to_raylib, BDRY_MAP},
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

impl RegId for AreaId {
    fn new() -> Self {
        Self(0)
    }

    fn increment(self) -> Self {
        let AreaId(id) = self;
        Self(id + 1)
    }
}

impl Default for AreaId {
    fn default() -> Self {
        AreaId(-1)
    }
}

pub struct Area {
    pub id: AreaId,
    pub area_type: AreaType,
    pub screen_rect: Rectangle,
    pub screen_pos: Vector2<f64>,
    pub active: bool,
    pub anchor: RenderAnchor,
    pub texture: RenderTexture2D,
    pub ui: Vec<UiId>,
    hovered: bool,
}

impl Area {
    pub fn new(
        area_type: AreaType,
        id: AreaId,
        screen_rect: Rectangle,
        screen_pos: Vector2<f64>,
        texture: RenderTexture2D,
        rl: &mut RaylibHandle,
    ) -> Self {
        let mut out = Self {
            id,
            area_type,
            screen_rect,
            screen_pos,
            active: false,
            anchor: RenderAnchor::Left,
            texture,
            ui: vec![],
            hovered: false,
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
            AreaType::Empty => {
                out.build_empty(rl);
            }
        }
        out
    }

    pub fn draw(&mut self, d: &mut RaylibDrawHandle, t: &RaylibThread) {
        let offset = Vector2::<f64>::new(
            ((self.texture.width() as f32 - self.screen_rect.width) / 2.0) as f64,
            ((self.texture.height() as f32 - self.screen_rect.height) / 2.0) as f64,
        );
        {
            let mut td = d.begin_texture_mode(t, &mut self.texture);
            let s = STYLE.read().unwrap();
            td.clear_background(s.bg_color);
            UI_MAP.with_borrow_mut(|ui_map| {
                for id in &self.ui {
                    let ui = &mut ui_map[*id];
                    match self.area_type {
                        AreaType::Empty | AreaType::Viewport3d => {
                            ui.move_relative(-offset);
                        }
                        _ => {}
                    }
                    ui.draw(&mut td, t);
                    match self.area_type {
                        AreaType::Empty | AreaType::Viewport3d => {
                            ui.move_relative(offset);
                        }
                        _ => {}
                    }
                }
            });
        }
        let mut draw_rect = self.screen_rect;
        draw_rect.y = -draw_rect.height;
        draw_rect.height *= -1.0;
        match self.anchor {
            RenderAnchor::Left => {}
            RenderAnchor::Center => {
                //draw_rect.x = (self.texture.width() as f32 - self.screen_rect.width) / 2.0;
                //draw_rect.y = (self.texture.height() as f32 - self.screen_rect.height) / 2.0;
            }
            RenderAnchor::Right => {
                //draw_rect.x = self.texture.width() as f32 - self.screen_rect.width;
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
            let bdry = &bdry_map[*bdry_id];
            is_true =
                bdry.orientation == BoundaryOrientation::Vertical && bdry.side1.contains(&self.id);
        });
        is_true
    }

    pub fn is_right_of(&self, bdry_id: &BoundaryId) -> bool {
        let mut is_true = true;
        BDRY_MAP.with_borrow(|bdry_map| {
            let bdry = &bdry_map[*bdry_id];
            is_true =
                bdry.orientation == BoundaryOrientation::Vertical && bdry.side2.contains(&self.id);
        });
        is_true
    }

    pub fn is_below(&self, bdry_id: &BoundaryId) -> bool {
        let mut is_true = true;
        BDRY_MAP.with_borrow(|bdry_map| {
            let bdry = &bdry_map[*bdry_id];
            is_true =
                bdry.orientation == BoundaryOrientation::Vertical && bdry.side2.contains(&self.id);
            bdry.orientation == BoundaryOrientation::Horizontal && bdry.side2.contains(&self.id)
        });
        is_true
    }

    pub fn is_above(&self, bdry_id: &BoundaryId) -> bool {
        let mut is_true = true;
        BDRY_MAP.with_borrow(|bdry_map| {
            let bdry = &bdry_map[*bdry_id];
            is_true =
                bdry.orientation == BoundaryOrientation::Horizontal && bdry.side1.contains(&self.id)
        });
        is_true
    }

    pub fn further_down_bdry_tree(
        &self,
        bdry_map: &Registry<BoundaryId, Boundary>,
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

    fn build_empty(&mut self, rl: &mut RaylibHandle) {
        let mut text = Box::new(ui::text::Text::new());
        text.set_pos(Vector2::<f64>::new(
            self.texture.width() as f64 / 2.0,
            self.texture.height() as f64 / 2.0,
        ));
        text.align = TextAlignment::CENTER;
        let AreaId(n) = self.id;
        text.set_text(format!("{:?}", n), rl);
        text.set_font_size(40.0, rl);
        //text.set_on_mouse_enter(Box::new(|| text.set_font_size(20.0, rl)));
        self.anchor = RenderAnchor::Center;
    }
}

impl fmt::Debug for Area {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Area")
            .field("id", &self.id)
            .field("area_type", &self.area_type)
            .field("screen_rect", &self.screen_rect)
            .field("screen_pos", &self.screen_pos)
            .field("active", &self.active)
            .field("anchor", &self.anchor)
            .field("texture", &self.texture)
            .field("ui", &self.ui.len())
            .finish()
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
        UI_MAP.with_borrow_mut(|ui_map| {
            if self.contains_point(mouse_pos) {
                if !self.hovered {
                    // onMouseEnter if it exists
                }
                self.hovered = true;
                for id in &self.ui {
                    let ui = &mut ui_map[*id];
                    ui.receive_mouse_pos(mouse_pos - self.screen_pos);
                }
            } else {
                if self.hovered {
                    // onMouseExit if it exists
                }
                self.hovered = false;
                for id in &self.ui {
                    let ui = &mut ui_map[*id];
                    // If the mouse isn't in the active area, treat it as being outside the entire
                    // window
                    ui.receive_mouse_pos(Vector2::<f64>::new(-100.0, -100.0));
                }
            }
        });
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>) {
        if self.contains_point(mouse_pos) {
            UI_MAP.with_borrow_mut(|ui_map| {
                for id in &self.ui {
                    let ui = &mut ui_map[*id];
                    ui.receive_mouse_down(mouse_pos - self.screen_pos);
                }
            });
        }
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>) {
        if self.contains_point(mouse_pos) {
            UI_MAP.with_borrow_mut(|ui_map| {
                for id in &mut self.ui {
                    let ui = &mut ui_map[*id];
                    ui.receive_mouse_up(mouse_pos - self.screen_pos);
                }
            });
        }
    }

    fn receive_mouse_wheel(&mut self, mouse_pos: Vector2<f64>, movement: f64) {
        if self.contains_point(mouse_pos) {
            UI_MAP.with_borrow_mut(|ui_map| {
                for id in &self.ui {
                    let ui = &mut ui_map[*id];
                    ui.receive_mouse_up(mouse_pos - self.screen_pos);
                }
            });
        }
    }
}

impl PartialEq for Area {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

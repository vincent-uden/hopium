use core::fmt;

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
    event::Event,
    images::ImageId,
    modes::MousePress,
    ui::{self, text::TextAlignment, Drawable, MouseEventHandler, Ui},
};
use crate::{registry::RegId, STYLES};
use crate::{
    registry::Registry,
    ui::style::{StyleId, StyleType},
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

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy, Deserialize, Serialize)]
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

#[derive(Serialize)]
pub struct Area {
    pub id: AreaId,
    pub area_type: AreaType,
    #[serde(skip_serializing)]
    pub screen_rect: Rectangle,
    #[serde(skip_serializing)]
    pub screen_pos: Vector2<f64>,
    #[serde(skip_serializing)]
    pub active: bool,
    #[serde(skip_serializing)]
    pub anchor: RenderAnchor,
    #[serde(skip_serializing)]
    pub texture: RenderTexture2D,
    #[serde(skip_serializing)]
    pub ui: Vec<Box<dyn Ui>>,
    #[serde(skip_serializing)]
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
        out.build(rl);
        out
    }

    pub fn build(&mut self, rl: &mut RaylibHandle) {
        match self.area_type {
            AreaType::Viewport3d => {
                self.build_viewport_3d();
            }
            AreaType::ToolSelection => {
                self.build_tool_selection();
            }
            AreaType::ContraintSelection => {
                self.build_constraint_selection(rl);
            }
            AreaType::SketchViewer => {
                self.build_sketch_viewer(rl);
            }
            AreaType::Empty => {
                self.build_empty(rl);
            }
        }
    }

    pub fn draw(&mut self, d: &mut RaylibDrawHandle, t: &RaylibThread) {
        let offset = Vector2::<f64>::new(
            ((self.texture.width() as f32 - self.screen_rect.width) / 2.0) as f64,
            ((self.texture.height() as f32 - self.screen_rect.height) / 2.0) as f64,
        );
        {
            let mut td = d.begin_texture_mode(t, &mut self.texture);
            let s = &STYLES.read().unwrap()[StyleId(StyleType::Area)];
            td.clear_background(s.bg_color);
            for ui in &mut self.ui {
                if let AreaType::Viewport3d = self.area_type {
                    ui.move_relative(-offset);
                }
                ui.draw(&mut td, t);
                if let AreaType::Viewport3d = self.area_type {
                    ui.move_relative(offset);
                }
            }
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

    pub fn further_up_bdry_tree(
        &self,
        bdry_map: &Registry<BoundaryId, Boundary>,
    ) -> Vec<BoundaryId> {
        let mut out = vec![];
        for (id, bdry) in bdry_map.iter() {
            if bdry.side2.contains(&self.id) {
                out.push(*id);
            }
        }
        out
    }

    fn build_area_type_picker(&mut self, rl: &mut RaylibHandle) {
        let mut picker = Box::new(ui::dropdown::DropDown::new());
        picker.set_contents(
            String::from("Area Type"),
            vec![
                String::from("Empty"),
                String::from("Constraint Selection"),
                String::from("Sketch Viewer"),
            ],
            rl,
        );
        picker.option_events = vec![
            Event::ChangeAreaType {
                id: self.id,
                area_type: AreaType::Empty,
            },
            Event::ChangeAreaType {
                id: self.id,
                area_type: AreaType::ContraintSelection,
            },
            Event::ChangeAreaType {
                id: self.id,
                area_type: AreaType::SketchViewer,
            },
        ];
        self.ui.push(picker);
    }

    fn build_viewport_3d(&mut self) {
        todo!()
    }
    fn build_tool_selection(&mut self) {
        todo!()
    }
    fn build_constraint_selection(&mut self, rl: &mut RaylibHandle) {
        self.ui.clear();
        let mut constraint_selector = Box::new(ui::constraint_selector::ConstraintSelector::new());
        constraint_selector.set_pos(Vector2::new(40.0, 40.0));
        self.ui.push(constraint_selector);
        self.build_area_type_picker(rl);
    }
    fn build_sketch_viewer(&mut self, rl: &mut RaylibHandle) {
        self.ui.clear();
        let sketch_viewer = Box::new(ui::sketchviewer::SketchViewer::new());
        self.ui.push(sketch_viewer);
        self.build_area_type_picker(rl);
    }

    fn build_empty(&mut self, rl: &mut RaylibHandle) {
        self.ui.clear();
        let mut text = Box::new(ui::text::Text::new());
        text.set_pos(Vector2::<f64>::new(
            self.screen_rect.width as f64 / 2.0,
            self.screen_rect.height as f64 / 2.0,
        ));
        text.align = TextAlignment::Center;
        let AreaId(n) = self.id;
        text.set_text(format!("{:?}", n), rl);
        text.set_font_size(40.0, rl);
        self.ui.push(text);

        let mut icon = Box::new(ui::icon::Icon::new());
        icon.set_image(ImageId::IconCoincident);
        icon.set_pos(Vector2::new(200.0, 10.0));
        icon.size = Vector2::new(40.0, 40.0);
        self.ui.push(icon);

        self.build_area_type_picker(rl);
        self.anchor = RenderAnchor::Left;
    }

    fn to_local_space(&self, mouse_pos: Vector2<f64>) -> Vector2<f64> {
        let anchor_offset = match self.anchor {
            RenderAnchor::Left => -self.screen_pos,
            RenderAnchor::Center => Vector2::<f64>::new(
                ((self.texture.width() as f32 - self.screen_rect.width) / 2.0
                    - self.screen_pos.x as f32) as f64,
                ((self.texture.height() as f32 - self.screen_rect.height) / 2.0
                    - self.screen_pos.y as f32) as f64,
            ),
            RenderAnchor::Right => Vector2::new(
                ((self.texture.width() as f32 - self.screen_rect.width) / 2.0
                    - self.screen_pos.x as f32) as f64,
                -self.screen_pos.y,
            ),
        };

        mouse_pos + anchor_offset
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
        if self.contains_point(mouse_pos) {
            self.hovered = true;
            let local = self.to_local_space(mouse_pos);
            for ui in &mut self.ui {
                ui.receive_mouse_pos(local);
            }
        } else {
            self.hovered = false;
            for ui in &mut self.ui {
                // If the mouse isn't in the active area, treat it as being outside the entire
                // window
                ui.receive_mouse_pos(Vector2::<f64>::new(-100.0, -100.0));
            }
        }
    }

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>, press: &MousePress) {
        if self.contains_point(mouse_pos) {
            for ui in &mut self.ui {
                ui.receive_mouse_down(mouse_pos - self.screen_pos, press);
            }
        }
    }

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>, press: &MousePress) {
        if self.contains_point(mouse_pos) {
            for ui in &mut self.ui {
                ui.receive_mouse_up(mouse_pos - self.screen_pos, press);
            }
        }
    }

    fn receive_mouse_wheel(&mut self, mouse_pos: Vector2<f64>, movement: f64) {
        if self.contains_point(mouse_pos) {
            for ui in &mut self.ui {
                ui.receive_mouse_wheel(mouse_pos - self.screen_pos, movement);
            }
        }
    }
}

impl PartialEq for Area {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

use std::cell::RefCell;
use std::collections::HashMap;

use nalgebra::Vector2;
use raylib::{
    color::Color,
    drawing::{RaylibDraw, RaylibDrawHandle, RaylibTextureModeExt},
    ffi::KeyboardKey,
    math::Rectangle,
    texture::{RaylibTexture2D, RenderTexture2D},
    RaylibHandle, RaylibThread,
};
use serde::{Deserialize, Serialize};

pub fn to_raylib(x: Vector2<f64>) -> raylib::math::Vector2 {
    raylib::math::Vector2 {
        x: x.x as f32,
        y: x.y as f32,
    }
}

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
pub struct AreaId(i64);

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
    id: AreaId,
    area_type: AreaType,
    screen_rect: Rectangle,
    screen_pos: Vector2<f64>,
    active: bool,
    anchor: RenderAnchor,
    texture: RenderTexture2D,
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
            td.clear_background(Color::BLACK);
            // TODO: Translate and draw contained ut when it exists
        }
        let mut draw_rect = self.screen_rect.clone();
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

    pub fn further_down_bdry_tree(&self) -> Option<BoundaryId> {
        let mut out = None;
        BDRY_MAP.with_borrow(|bdry_map| {
            for (id, bdry) in bdry_map.iter() {
                if bdry.side1.contains(&self.id) {
                    out = Some(*id);
                }
            }
        });
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
        todo!()
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

pub trait MouseEventHandler {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool;

    fn receive_mouse_pos(&mut self, mouse_pos: Vector2<f64>);

    fn receive_mouse_down(&mut self, mouse_pos: Vector2<f64>);

    fn receive_mouse_up(&mut self, mouse_pos: Vector2<f64>);

    fn receive_mouse_wheel(&mut self, mouse_pos: Vector2<f64>, movement: f64);

    // TODO: Set mouse event listeners
}

#[derive(Debug, Deserialize, Serialize, PartialEq, Eq)]
pub enum BoundaryOrientation {
    Horizontal,
    Vertical,
}

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy)]
pub struct BoundaryId(i64);

impl BoundaryId {
    pub fn increment(self) -> Self {
        let BoundaryId(id) = self;
        Self(id + 1)
    }
}

impl Default for BoundaryId {
    fn default() -> Self {
        BoundaryId(-1)
    }
}

#[derive(Debug)]
pub struct Boundary {
    id: BoundaryId,
    orientation: BoundaryOrientation,
    active: bool,
    thickness: i32,
    // Left / Up
    side1: Vec<AreaId>,
    // Down / Right
    side2: Vec<AreaId>,
}

impl Boundary {
    pub fn new(id: BoundaryId, orientation: BoundaryOrientation) -> Self {
        Self {
            id,
            orientation,
            active: false,
            thickness: 3,
            side1: vec![],
            side2: vec![],
        }
    }

    pub fn draw(&self, d: &mut RaylibDrawHandle, t: &RaylibThread) {
        AREA_MAP.with_borrow(|area_map| {
            let start_pos = area_map[&self.side2[0]].screen_pos.clone();
            let mut end_pos = start_pos.clone();
            match self.orientation {
                BoundaryOrientation::Horizontal => {
                    end_pos.x += self.extent();
                }
                BoundaryOrientation::Vertical => {
                    end_pos.y += self.extent();
                }
            }
            d.draw_line_ex(
                to_raylib(start_pos),
                to_raylib(end_pos),
                self.thickness as f32,
                Color::WHITE,
            );
        });
    }

    pub fn extent(&self) -> f64 {
        let mut total = 0.0;
        AREA_MAP.with_borrow(|area_map| match self.orientation {
            BoundaryOrientation::Horizontal => {
                for area_id in &self.side1 {
                    let area = &area_map[area_id];
                    total += area.screen_rect.height;
                }
            }
            BoundaryOrientation::Vertical => {
                for area_id in &self.side1 {
                    let area = &area_map[area_id];
                    total += area.screen_rect.height;
                }
            }
        });
        total as f64
    }

    pub fn distance_to_point(&self, p: Vector2<f64>) -> f64 {
        AREA_MAP.with_borrow(|area_map| {
            let area = &area_map[&self.side2[0]];
            match self.orientation {
                BoundaryOrientation::Horizontal => {
                    if p.y > area.screen_pos.y && p.y < area.screen_pos.y + self.extent() {
                        return (area.screen_pos.x - p.x).abs();
                    }
                }
                BoundaryOrientation::Vertical => {
                    if p.x > area.screen_pos.x && p.x < area.screen_pos.x + self.extent() {
                        return (area.screen_pos.y - p.y).abs();
                    }
                }
            }
            f64::INFINITY
        })
    }

    pub fn can_collapse(&self) -> bool {
        self.side1.len() == 1 && self.side2.len() == 1
    }

    pub fn collapse(&mut self) {
        // When collapsing, the area on side2 is always deleted
        // There might be another boundary to the right/down of side2, it must be
        // connected with area in side1
        if self.can_collapse() {
            AREA_MAP.with_borrow_mut(|area_map| {
                let deleted_dims = area_map[&self.side2[0]].screen_rect;
                {
                    let remaining_area = area_map.get_mut(&self.side1[0]).unwrap();
                    match self.orientation {
                        BoundaryOrientation::Horizontal => {
                            remaining_area.screen_rect.height += deleted_dims.height;
                        }
                        BoundaryOrientation::Vertical => {
                            remaining_area.screen_rect.width += deleted_dims.width;
                        }
                    }
                }
                let to_delete = area_map.get(&self.side2[0]).unwrap();
                if let Some(bdry_id) = to_delete.further_down_bdry_tree() {
                    BDRY_MAP.with_borrow_mut(|bdry_map| {
                        let bdry = bdry_map.get_mut(&bdry_id).unwrap();
                        bdry.side1.push(self.side1[0]);
                    })
                }
            });
        }
    }

    pub fn delete_area(&mut self, to_delete: &AreaId) {
        match self.side1.iter().position(|x| *x == *to_delete) {
            Some(i) => {
                self.side1.remove(i);
            }
            None => {}
        }
        match self.side2.iter().position(|x| *x == *to_delete) {
            Some(i) => {
                self.side2.remove(i);
            }
            None => {}
        }
    }

    pub fn move_boundary(&mut self, pos: Vector2<f64>) {
        AREA_MAP.with_borrow_mut(|area_map| {
            // TODO: Ensure minimum extents
            let bdry_pos = area_map[&self.side2[0]].screen_pos;
            let diff = pos - bdry_pos;
            match self.orientation {
                BoundaryOrientation::Horizontal => {
                    for area_id in &self.side1 {
                        let area = area_map.get_mut(&area_id).unwrap();
                        area.screen_rect.height += diff.y as f32;
                    }
                    for area_id in &self.side2 {
                        let area = area_map.get_mut(&area_id).unwrap();
                        area.screen_rect.height -= diff.y as f32;
                    }
                }
                BoundaryOrientation::Vertical => {
                    for area_id in &self.side1 {
                        let area = area_map.get_mut(&area_id).unwrap();
                        area.screen_rect.width += diff.x as f32;
                    }
                    for area_id in &self.side2 {
                        let area = area_map.get_mut(&area_id).unwrap();
                        area.screen_rect.width -= diff.x as f32;
                    }
                }
            }
        })
    }
}

impl PartialEq for Boundary {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

thread_local!(pub static AREA_MAP: RefCell<HashMap<AreaId, Area>> = RefCell::new(HashMap::new()));
thread_local!(pub static BDRY_MAP: RefCell<HashMap<BoundaryId, Boundary>> = RefCell::new(HashMap::new()));

#[derive(Debug)]
pub struct Renderer<'a> {
    screen_w: i32,
    screen_h: i32,
    mouse_bdry_tol: f64,
    mouse_pos: Vector2<f64>,
    grabbed: Option<BoundaryId>,
    next_area_id: AreaId,
    next_bdry_id: BoundaryId,
    rl: &'a mut RaylibHandle,
    t: &'a RaylibThread,
    pub running: bool,
}

impl<'a> Renderer<'a> {
    pub fn new(
        screen_w: i32,
        screen_h: i32,
        rl: &'a mut RaylibHandle,
        t: &'a RaylibThread,
    ) -> Self {
        AREA_MAP.with(|areas| {
            areas.borrow_mut().insert(
                AreaId(0),
                Area::new(
                    AreaType::Empty,
                    AreaId(0),
                    Rectangle {
                        x: 0.0,
                        y: 0.0,
                        width: screen_w as f32,
                        height: screen_h as f32,
                    },
                    Vector2::default(),
                    rl.load_render_texture(t, screen_w as u32, screen_h as u32)
                        .unwrap(),
                ),
            );
        });
        let out = Self {
            screen_w,
            screen_h,
            mouse_bdry_tol: 5.0,
            mouse_pos: Vector2::default(),
            grabbed: None,
            next_area_id: AreaId(1),
            next_bdry_id: BoundaryId(0),
            rl,
            t,
            running: true,
        };
        out
    }

    pub fn draw(&mut self) {
        let mut d = self.rl.begin_drawing(self.t);
        d.clear_background(Color::BLACK);

        AREA_MAP.with_borrow_mut(|areas| {
            for area in areas.values_mut() {
                area.draw(&mut d, self.t);
            }
        });

        BDRY_MAP.with_borrow(|bdrys| {
            for bdry in bdrys.values() {
                bdry.draw(&mut d, self.t);
            }
        });

        d.draw_fps(self.screen_w - 100, 10);
    }

    pub fn update(&mut self) {
        if self.rl.is_key_pressed(KeyboardKey::KEY_Q) {
            self.running = false;
        }
    }

    fn split_area(&mut self, mouse_pos: Vector2<f64>, orientation: BoundaryOrientation) {
        AREA_MAP.with_borrow_mut(|area_map| {
            if let Some(to_split) = area_map.get_mut(&self.find_area(mouse_pos).unwrap_or_default())
            {
                let new_rect = match orientation {
                    BoundaryOrientation::Horizontal => Rectangle {
                        x: 0.0,
                        y: 0.0,
                        width: to_split.screen_rect.width / 2.0,
                        height: to_split.screen_rect.height,
                    },
                    BoundaryOrientation::Vertical => Rectangle {
                        x: 0.0,
                        y: 0.0,
                        width: to_split.screen_rect.width,
                        height: to_split.screen_rect.height / 2.0,
                    },
                };
                let new_pos = match orientation {
                    BoundaryOrientation::Horizontal => Vector2::<f64>::new(
                        to_split.screen_pos.x + to_split.screen_rect.width as f64 / 2.0,
                        to_split.screen_pos.y,
                    ),
                    BoundaryOrientation::Vertical => Vector2::<f64>::new(
                        to_split.screen_pos.x,
                        to_split.screen_pos.y + to_split.screen_rect.height as f64 / 2.0,
                    ),
                };
                let new_area = Area::new(
                    AreaType::Empty,
                    self.next_area_id,
                    new_rect,
                    new_pos,
                    self.rl
                        .load_render_texture(self.t, self.screen_w as u32, self.screen_h as u32)
                        .unwrap(),
                );
                self.next_area_id = self.next_area_id.increment();
                to_split.screen_rect.width /= 2.0;

                let mut bdry = Boundary::new(self.next_bdry_id, orientation);
                self.next_bdry_id = self.next_bdry_id.increment();
                bdry.side1.push(to_split.id);
                bdry.side2.push(new_area.id);

                BDRY_MAP.with_borrow_mut(|bdry_map| {
                    if let Some(existing_bdry) =
                        bdry_map.get_mut(&to_split.further_down_bdry_tree().unwrap_or_default())
                    {
                        existing_bdry.side1.retain(|id| *id != to_split.id);
                        existing_bdry.side1.push(new_area.id);
                    }
                    bdry_map.insert(bdry.id, bdry);
                });
                area_map.insert(new_area.id, new_area);
            }
        });
    }

    pub fn collapse_boundary(&mut self, mouse_pos: Vector2<f64>) {
        match self.find_boundary(mouse_pos, self.mouse_bdry_tol) {
            Some(hovered) => {
                BDRY_MAP.with_borrow_mut(|bdry_map| {
                    let mut bdry = bdry_map.remove(&hovered).unwrap();
                    if bdry.side2.len() != 1 {
                        panic!("Cannot delete boundary with more than one child area");
                    }
                    bdry.collapse();
                    AREA_MAP.with_borrow_mut(|area_map| area_map.remove(&bdry.side2[0]));
                });
            }
            None => {}
        }
    }

    fn find_boundary(&self, mouse_pos: Vector2<f64>, radius: f64) -> Option<BoundaryId> {
        let mut out = None;
        let mut closest_dist = f64::INFINITY;
        BDRY_MAP.with_borrow(|bdry_map| {
            for (id, bdry) in bdry_map.iter() {
                let dist = bdry.distance_to_point(mouse_pos);
                if dist < closest_dist {
                    out = Some(id.clone());
                    closest_dist = dist;
                }
            }
        });
        if closest_dist < radius {
            out
        } else {
            None
        }
    }

    fn find_area(&self, mouse_pos: Vector2<f64>) -> Option<AreaId> {
        let mut out = None;
        AREA_MAP.with_borrow(|area_map| {
            for (id, area) in area_map.iter() {
                if area.contains_point(mouse_pos) {
                    out = Some(id.clone());
                }
            }
        });
        out
    }
}

impl MouseEventHandler for Renderer<'_> {
    fn contains_point(&self, mouse_pos: Vector2<f64>) -> bool {
        todo!()
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

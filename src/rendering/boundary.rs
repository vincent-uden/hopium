use nalgebra::Vector2;
use raylib::{
    drawing::{RaylibDraw, RaylibDrawHandle},
    RaylibHandle, RaylibThread,
};
use serde::{Deserialize, Serialize};

use crate::{
    registry::{RegId, Registry},
    ui::style::{StyleId, StyleType},
    STYLES,
};

use super::{
    area::AreaId,
    renderer::{to_raylib, AREA_MAP},
};

#[derive(Debug, Deserialize, Serialize, PartialEq, Eq)]
pub enum BoundaryOrientation {
    Horizontal,
    Vertical,
}

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy, Deserialize, Serialize)]
pub struct BoundaryId(pub i64);

impl RegId for BoundaryId {
    fn new() -> Self {
        Self(0)
    }

    fn increment(self) -> Self {
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
    pub id: BoundaryId,
    pub orientation: BoundaryOrientation,
    pub active: bool,
    pub thickness: i32,
    pub hovered_thickness: i32,
    pub hovered: bool,
    // Left / Up
    pub side1: Vec<AreaId>,
    // Down / Right
    pub side2: Vec<AreaId>,
}

impl Boundary {
    pub fn new(id: BoundaryId, orientation: BoundaryOrientation) -> Self {
        Self {
            id,
            orientation,
            active: false,
            thickness: 3,
            hovered_thickness: 6,
            side1: vec![],
            side2: vec![],
            hovered: false,
        }
    }

    pub fn draw(&self, d: &mut RaylibDrawHandle, t: &RaylibThread) {
        AREA_MAP.with_borrow(|area_map| {
            let start_pos = area_map[self.side2[0]].screen_pos;
            let mut end_pos = start_pos;
            match self.orientation {
                BoundaryOrientation::Horizontal => {
                    end_pos.x += self.extent();
                }
                BoundaryOrientation::Vertical => {
                    end_pos.y += self.extent();
                }
            }
            let s = &STYLES.read().unwrap()[StyleId(StyleType::Boundary)];
            d.draw_line_ex(
                to_raylib(start_pos),
                to_raylib(end_pos),
                if self.hovered {
                    self.hovered_thickness as f32
                } else {
                    self.thickness as f32
                },
                s.color,
            );
        });
    }

    pub fn extent(&self) -> f64 {
        let mut total = 0.0;
        AREA_MAP.with_borrow(|area_map| match self.orientation {
            BoundaryOrientation::Horizontal => {
                let mut total1 = 0.0;
                let mut total2 = 0.0;
                for area_id in &self.side1 {
                    let area = &area_map[*area_id];
                    total1 += area.screen_rect.width;
                }
                for area_id in &self.side2 {
                    let area = &area_map[*area_id];
                    total2 += area.screen_rect.width;
                }
                total = total1.max(total2);
            }
            BoundaryOrientation::Vertical => {
                let mut total1 = 0.0;
                let mut total2 = 0.0;
                for area_id in &self.side1 {
                    let area = &area_map[*area_id];
                    total1 += area.screen_rect.height;
                }
                for area_id in &self.side2 {
                    let area = &area_map[*area_id];
                    total2 += area.screen_rect.height;
                }
                total = total1.max(total2);
            }
        });
        total as f64
    }

    pub fn distance_to_point(&self, p: Vector2<f64>) -> f64 {
        AREA_MAP.with_borrow(|area_map| {
            let area = &area_map[self.side2[0]];
            match self.orientation {
                BoundaryOrientation::Vertical => {
                    if p.y > area.screen_pos.y && p.y < area.screen_pos.y + self.extent() {
                        return (area.screen_pos.x - p.x).abs();
                    }
                }
                BoundaryOrientation::Horizontal => {
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

    pub fn collapse(
        &mut self,
        bdry_map: &mut Registry<BoundaryId, Boundary>,
        rl: &mut RaylibHandle,
    ) {
        // When collapsing, the area on side2 is always deleted
        // There might be another boundary to the right/down of side2, it must be
        // connected with area in side1
        if self.can_collapse() {
            AREA_MAP.with_borrow_mut(|area_map| {
                let deleted_dims = area_map[self.side2[0]].screen_rect;
                {
                    let remaining_area = &mut area_map[self.side1[0]];
                    match self.orientation {
                        BoundaryOrientation::Horizontal => {
                            remaining_area.screen_rect.height += deleted_dims.height;
                        }
                        BoundaryOrientation::Vertical => {
                            remaining_area.screen_rect.width += deleted_dims.width;
                        }
                    }
                    remaining_area.build(rl);
                }
                let to_delete = &area_map[self.side2[0]];
                if let Some(bdry_id) = to_delete.further_down_bdry_tree(bdry_map).first() {
                    let bdry = &mut bdry_map[*bdry_id];
                    if !bdry.side1.contains(&self.side1[0]) {
                        bdry.side1.push(self.side1[0]);
                    }
                }
                for bdry in bdry_map.values_mut() {
                    bdry.side1.retain(|x| *x != to_delete.id);
                    bdry.side2.retain(|x| *x != to_delete.id);
                }
            })
        }
    }

    pub fn delete_area(&mut self, to_delete: &AreaId) {
        if let Some(i) = self.side1.iter().position(|x| *x == *to_delete) {
            self.side1.remove(i);
        }
        if let Some(i) = self.side2.iter().position(|x| *x == *to_delete) {
            self.side2.remove(i);
        }
    }

    pub fn move_boundary(&mut self, pos: Vector2<f64>) {
        // TODO: Ensure minimum extents
        AREA_MAP.with_borrow_mut(|area_map| {
            let bdry_pos = area_map[self.side2[0]].screen_pos;
            let diff = pos - bdry_pos;
            match self.orientation {
                BoundaryOrientation::Horizontal => {
                    for area_id in &self.side1 {
                        let area = &mut area_map[*area_id];
                        area.screen_rect.height += diff.y as f32;
                    }
                    for area_id in &self.side2 {
                        let area = &mut area_map[*area_id];
                        area.screen_rect.height -= diff.y as f32;
                        area.screen_pos.y += diff.y;
                    }
                }
                BoundaryOrientation::Vertical => {
                    for area_id in &self.side1 {
                        let area = &mut area_map[*area_id];
                        area.screen_rect.width += diff.x as f32;
                    }
                    for area_id in &self.side2 {
                        let area = &mut area_map[*area_id];
                        area.screen_rect.width -= diff.x as f32;
                        area.screen_pos.x += diff.x;
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

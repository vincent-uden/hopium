use nalgebra::Vector2;
use raylib::math::Rectangle;
use serde::{Deserialize, Serialize};

#[derive(Debug, Deserialize, Serialize)]
enum RenderAnchor {
    Left,
    Center,
    Right,
}

#[derive(Debug, Deserialize, Serialize)]
enum AreaType {
    Empty,
    Viewport3d,
    ToolSelection,
    ContraintSelection,
    SketchViewer,
}

#[derive(Debug)]
struct Area<'a> {
    id: i64,
    area_type: AreaType,
    screen_rect: Rectangle,
    screen_pos: Vector2<f64>,
    left_bdry: Option<&'a Boundary<'a>>,
    right_bdry: Option<&'a Boundary<'a>>,
    up_bdry: Option<&'a Boundary<'a>>,
    down_bdry: Option<&'a Boundary<'a>>,
    active: bool,
    anchor: RenderAnchor,
}

impl Area<'_> {
    pub fn draw(&self) {
        todo!()
    }

    pub fn delete_boundary(&mut self, bdry: &Boundary) {
        todo!()
    }

    pub fn delete_this_from_boundaries(&mut self) {
        todo!()
    }

    pub fn is_left_of(bdry: &Boundary) -> bool {
        todo!()
    }

    pub fn is_right_of(bdry: &Boundary) -> bool {
        todo!()
    }

    pub fn is_below(bdry: &Boundary) -> bool {
        todo!()
    }

    pub fn is_above(bdry: &Boundary) -> bool {
        todo!()
    }
}

impl MouseEventHandler for Area<'_> {
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

impl PartialEq for Area<'_> {
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

#[derive(Debug, Deserialize, Serialize)]
enum BoundaryOrientation {
    Horizontal,
    Vertical,
}
#[derive(Debug)]
struct Boundary<'a> {
    id: i64,
    orientation: BoundaryOrientation,
    active: bool,
    thickness: i32,
    // Left / Up
    side1: Option<&'a Area<'a>>,
    side2: Option<&'a Area<'a>>,
}

impl Boundary<'_> {
    pub fn draw(&self) {
        todo!()
    }

    pub fn extent(&self) -> f64 {
        todo!()
    }

    pub fn distance_to_point(&self, p: Vector2<f64>) -> f64 {
        todo!()
    }

    pub fn can_collapse() -> bool {
        todo!()
    }

    pub fn collapse(&mut self, to_delete: &Area) {
        todo!()
    }

    pub fn delete_area(&mut self, to_deleta: &Area) {
        todo!()
    }

    pub fn move_boundary(&mut self, pos: Vector2<f64>) {
        todo!()
    }
}

impl PartialEq for Boundary<'_> {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

#[derive(Debug)]
pub struct Renderer<'a> {
    areas: Vec<Area<'a>>,
    bdrys: Vec<Boundary<'a>>,
    screen_w: i32,
    screen_h: i32,
    mouse_bdry_tol: f64,
    mouse_pos: Vector2<f64>,
    grabbed: Option<&'a Boundary<'a>>,
}

impl Renderer<'_> {
    pub fn new(screen_w: i32, screen_h: i32) -> Self {
        Self {
            areas: vec![],
            bdrys: vec![],
            screen_w,
            screen_h,
            mouse_bdry_tol: 5.0,
            mouse_pos: Vector2::default(),
            grabbed: None,
        }
    }

    pub fn draw(&self) {
        todo!()
    }

    pub fn split_area_horizontal(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    pub fn split_area_vertical(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
    }

    pub fn collapse_boundary(&mut self, mouse_pos: Vector2<f64>) {
        todo!()
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

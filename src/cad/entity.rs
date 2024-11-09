use std::f64::consts::PI;

use log::{debug, info};
use nalgebra::{Rotation2, Vector2};
use serde::{Deserialize, Serialize};

use crate::{app::State, registry::RegId, APP_STATE};

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash, Clone, Copy, Deserialize, Serialize)]
pub struct EntityId(pub i64);

impl RegId for EntityId {
    fn new() -> Self {
        Self(0)
    }

    fn increment(self) -> Self {
        let EntityId(id) = self;
        Self(id + 1)
    }
}

impl Default for EntityId {
    fn default() -> Self {
        EntityId(-1)
    }
}

#[derive(Debug, Deserialize, Serialize, Clone, Copy)]
pub enum FundamentalEntity {
    Point(Point),
    Line(Line),
    Circle(Circle),
}

fn vector_angle(a: Vector2<f64>) -> f64 {
    let angle = f64::atan2(a.y, a.x);
    if angle < 0.0 {
        angle + 2.0 * std::f64::consts::PI
    } else {
        angle
    }
}

impl FundamentalEntity {
    pub fn distance_to_position(&self, target: &Vector2<f64>) -> f64 {
        match self {
            FundamentalEntity::Point(p) => (p.pos - target).norm(),
            FundamentalEntity::Line(l) => {
                let ortho_a = target - project(target, &l.direction);
                let ortho_r = l.offset - project(&l.offset, &l.direction);
                (ortho_r - ortho_a).norm()
            }
            FundamentalEntity::Circle(c) => ((target - c.pos).norm() - c.radius).abs(),
        }
    }

    pub fn circle_from_three_coords(
        p1: &Vector2<f64>,
        p2: &Vector2<f64>,
        p3: &Vector2<f64>,
    ) -> Option<Self> {
        let temp = p2.norm_squared();
        let bc = (p1.x.powi(2) + p1.y.powi(2) - temp) / 2.0;
        let cd = (temp - p3.x.powi(2) - p3.y.powi(2)) / 2.0;
        let det = (p1.x - p2.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p2.y);
        if det.abs() < 1e-12 {
            return None;
        }

        let center = Vector2::new(
            (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) / det,
            ((p1.x - p2.x) * cd - (p2.x - p3.x) * bc) / det,
        );

        let radius = ((center.x - p1.x).powi(2) + (center.y - p1.y).powi(2)).sqrt();

        Some(FundamentalEntity::Circle(Circle {
            pos: center,
            radius,
        }))
    }
}

#[derive(Debug, Deserialize, Serialize, Clone, Copy)]
pub struct Point {
    pub pos: Vector2<f64>,
}

#[derive(Debug, Deserialize, Serialize, Clone, Copy)]
pub struct Line {
    pub offset: Vector2<f64>,
    pub direction: Vector2<f64>,
}

#[derive(Debug, Deserialize, Serialize, Clone, Copy)]
pub struct Circle {
    pub pos: Vector2<f64>,
    pub radius: f64,
}

pub fn project(a: &Vector2<f64>, b: &Vector2<f64>) -> Vector2<f64> {
    a.dot(b) / b.dot(b) * b
}

#[derive(Debug, Deserialize, Serialize, Clone, Copy)]
pub enum ConstraintType {
    Angle { x: f64 },
    Coincident,
    Colinear,            // Should be paired with a parallel constraint for line-line
    Distance { x: f64 }, // Should be paired with a parallel constraint for line-line
    Horizontal,
    Parallel,
    Perpendicular,
    Tangent,
    Vertical,
}

#[derive(Debug, Deserialize, Serialize)]
pub struct BiConstraint {
    pub e1: EntityId,
    pub e2: EntityId,
    pub c: ConstraintType,
}

impl BiConstraint {
    pub fn new(e1: EntityId, e2: EntityId, c: ConstraintType) -> Self {
        Self { e1, e2, c }
    }

    pub fn possible(e1: &FundamentalEntity, e2: &FundamentalEntity, c: &ConstraintType) -> bool {
        match (e1, e2) {
            (FundamentalEntity::Point { .. }, FundamentalEntity::Point { .. }) => matches!(
                c,
                ConstraintType::Coincident
                    | ConstraintType::Distance { .. }
                    | ConstraintType::Vertical
                    | ConstraintType::Horizontal
            ),
            (FundamentalEntity::Point { .. }, FundamentalEntity::Line { .. }) => matches!(
                c,
                ConstraintType::Coincident | ConstraintType::Distance { .. }
            ),
            (FundamentalEntity::Point { .. }, FundamentalEntity::Circle { .. }) => matches!(
                c,
                ConstraintType::Coincident
                    | ConstraintType::Distance { .. }
                    | ConstraintType::Vertical
                    | ConstraintType::Horizontal
            ),
            (FundamentalEntity::Line { .. }, FundamentalEntity::Line { .. }) => matches!(
                c,
                ConstraintType::Parallel
                    | ConstraintType::Perpendicular
                    | ConstraintType::Colinear
                    | ConstraintType::Distance { .. }
                    | ConstraintType::Angle { .. }
            ),
            (FundamentalEntity::Circle { .. }, FundamentalEntity::Line { .. }) => matches!(
                c,
                ConstraintType::Coincident
                    | ConstraintType::Tangent
                    | ConstraintType::Distance { .. }
            ),
            (FundamentalEntity::Circle { .. }, FundamentalEntity::Circle { .. }) => matches!(
                c,
                ConstraintType::Coincident
                    | ConstraintType::Distance { .. }
                    | ConstraintType::Tangent
                    | ConstraintType::Vertical
                    | ConstraintType::Horizontal
            ),
            (_, _) => BiConstraint::possible(e2, e1, c),
        }
    }

    pub fn error(e1: &FundamentalEntity, e2: &FundamentalEntity, c: &ConstraintType) -> f64 {
        if Self::possible(e1, e2, c) {
            match (e1, e2) {
                (FundamentalEntity::Point(p1), FundamentalEntity::Point(p2)) => {
                    Self::error_pp(p1, p2, *c)
                }
                (FundamentalEntity::Point(p), FundamentalEntity::Line(l)) => {
                    Self::error_pl(p, l, *c)
                }
                (FundamentalEntity::Point(p), FundamentalEntity::Circle(ci)) => {
                    Self::error_pc(p, ci, *c)
                }
                (FundamentalEntity::Line(l1), FundamentalEntity::Line(l2)) => {
                    Self::error_ll(l1, l2, *c)
                }
                (FundamentalEntity::Line(l), FundamentalEntity::Circle(ci)) => {
                    Self::error_lc(l, ci, *c)
                }
                (FundamentalEntity::Circle(c1), FundamentalEntity::Circle(c2)) => {
                    Self::error_cc(c1, c2, *c)
                }
                _ => Self::error(e2, e1, c),
            }
        } else {
            0.0
        }
    }

    fn error_pp(p1: &Point, p2: &Point, c: ConstraintType) -> f64 {
        match c {
            ConstraintType::Coincident => (p1.pos - p2.pos).norm_squared(),
            ConstraintType::Horizontal => (p1.pos.y - p2.pos.y).powi(2),
            ConstraintType::Vertical => (p1.pos.x - p2.pos.x).powi(2),
            ConstraintType::Distance { x } => ((p1.pos - p2.pos).norm() - x).powi(2),
            _ => 0.0,
        }
    }

    fn error_pl(p: &Point, l: &Line, c: ConstraintType) -> f64 {
        let ortho_a = p.pos - project(&p.pos, &l.direction);
        let mut ortho_r = (p.pos - l.offset) - project(&(p.pos - l.offset), &l.direction);
        if ortho_r.dot(&ortho_a) < 0.0 {
            ortho_r = -ortho_r;
        }
        match c {
            ConstraintType::Coincident => (ortho_r + ortho_a).norm_squared(),
            ConstraintType::Distance { x } => ((ortho_r - ortho_a).norm() - x).powi(2),
            _ => 0.0,
        }
    }

    fn error_pc(p: &Point, ci: &Circle, c: ConstraintType) -> f64 {
        match c {
            ConstraintType::Coincident => ((p.pos - ci.pos).norm() - ci.radius).powi(2),
            ConstraintType::Horizontal => (p.pos.y - ci.pos.y).powi(2),
            ConstraintType::Vertical => (p.pos.x - ci.pos.x).powi(2),
            ConstraintType::Distance { x } => ((p.pos - ci.pos).norm() - x).powi(2),
            _ => 0.0,
        }
    }

    fn error_ll(l1: &Line, l2: &Line, c: ConstraintType) -> f64 {
        match c {
            ConstraintType::Parallel => (l1.direction.angle(&l2.direction)).powi(2),
            ConstraintType::Perpendicular => {
                (l1.direction.angle(&l2.direction) - std::f64::consts::PI / 2.0).powi(2)
            }
            ConstraintType::Colinear => {
                let ortho_1 = l1.offset - project(&l1.offset, &l1.direction);
                let ortho_2 = l2.offset - project(&l2.offset, &l2.direction);
                (ortho_1 - ortho_2).norm()
            }
            ConstraintType::Distance { x } => {
                let ortho_1 = l1.offset - project(&l1.offset, &l1.direction);
                let ortho_2 = l2.offset - project(&l2.offset, &l2.direction);
                ((ortho_1 - ortho_2).norm() - x).powi(2)
            }
            ConstraintType::Angle { x } => (l1.direction.angle(&l2.direction) - x).powi(2),
            _ => 0.0,
        }
    }

    fn error_lc(l: &Line, ci: &Circle, c: ConstraintType) -> f64 {
        let diff = ci.pos - l.offset;
        let ortho = diff - project(&diff, &l.direction);
        match c {
            // Doesnt seem to be working
            ConstraintType::Tangent => ((ortho).norm() - ci.radius).powi(2),
            ConstraintType::Distance { x } => ((ortho).norm() - x).powi(2),
            _ => 0.0,
        }
    }

    fn error_cc(c1: &Circle, c2: &Circle, c: ConstraintType) -> f64 {
        match c {
            ConstraintType::Coincident => (c1.pos - c2.pos).norm_squared(),
            ConstraintType::Horizontal => (c1.pos.y - c2.pos.y).powi(2),
            ConstraintType::Vertical => (c1.pos.x - c2.pos.x).powi(2),
            ConstraintType::Tangent => ((c1.pos - c2.pos).norm() - (c1.radius + c2.radius)).powi(2),
            ConstraintType::Distance { x } => ((c1.pos - c2.pos).norm() - x).powi(2),
            _ => 0.0,
        }
    }

    pub fn apply_grad_error(
        e1: &mut FundamentalEntity,
        e2: &FundamentalEntity,
        c: &ConstraintType,
        step_size: f64,
    ) {
        match e1 {
            FundamentalEntity::Point(p) => Self::apply_grad_error_p(p, e2, *c, step_size),
            FundamentalEntity::Line(l) => Self::apply_grad_error_l(l, e2, *c, step_size),
            FundamentalEntity::Circle(ci) => Self::apply_grad_error_c(ci, e2, *c, step_size),
        }
    }

    fn apply_grad_error_p(
        p1: &mut Point,
        e: &FundamentalEntity,
        c: ConstraintType,
        step_size: f64,
    ) {
        let h = 1e-6;
        let x_errors = [
            Self::error(
                &FundamentalEntity::Point(Point {
                    pos: p1.pos + Vector2::new(-h / 2.0, 0.0),
                }),
                e,
                &c,
            ),
            Self::error(
                &FundamentalEntity::Point(Point {
                    pos: p1.pos + Vector2::new(h / 2.0, 0.0),
                }),
                e,
                &c,
            ),
        ];
        let y_errors = [
            Self::error(
                &FundamentalEntity::Point(Point {
                    pos: p1.pos + Vector2::new(0.0, -h / 2.0),
                }),
                e,
                &c,
            ),
            Self::error(
                &FundamentalEntity::Point(Point {
                    pos: p1.pos + Vector2::new(0.0, h / 2.0),
                }),
                e,
                &c,
            ),
        ];

        let x_derivative = (x_errors[1] - x_errors[0]) / h;
        let y_derivative = (y_errors[1] - y_errors[0]) / h;
        let step = Vector2::new(x_derivative, y_derivative);
        p1.pos -= step * step_size;
    }

    fn apply_grad_error_l(l: &mut Line, e: &FundamentalEntity, c: ConstraintType, step_size: f64) {
        let h = 1e-4;
        let o_x_errors = [
            Self::error(
                &FundamentalEntity::Line(Line {
                    offset: l.offset + Vector2::new(-h / 2.0, 0.0),
                    direction: l.direction,
                }),
                e,
                &c,
            ),
            Self::error(
                &FundamentalEntity::Line(Line {
                    offset: l.offset + Vector2::new(h / 2.0, 0.0),
                    direction: l.direction,
                }),
                e,
                &c,
            ),
        ];
        let o_y_errors = [
            Self::error(
                &FundamentalEntity::Line(Line {
                    offset: l.offset + Vector2::new(0.0, -h / 2.0),
                    direction: l.direction,
                }),
                e,
                &c,
            ),
            Self::error(
                &FundamentalEntity::Line(Line {
                    offset: l.offset + Vector2::new(0.0, h / 2.0),
                    direction: l.direction,
                }),
                e,
                &c,
            ),
        ];
        let d_x_errors = [
            Self::error(
                &FundamentalEntity::Line(Line {
                    offset: l.offset,
                    direction: l.direction + Vector2::new(-h / 2.0, 0.0),
                }),
                e,
                &c,
            ),
            Self::error(
                &FundamentalEntity::Line(Line {
                    offset: l.offset,
                    direction: l.direction + Vector2::new(h / 2.0, 0.0),
                }),
                e,
                &c,
            ),
        ];
        let d_y_errors = [
            Self::error(
                &FundamentalEntity::Line(Line {
                    offset: l.offset,
                    direction: l.direction + Vector2::new(0.0, -h / 2.0),
                }),
                e,
                &c,
            ),
            Self::error(
                &FundamentalEntity::Line(Line {
                    offset: l.offset,
                    direction: l.direction + Vector2::new(0.0, h / 2.0),
                }),
                e,
                &c,
            ),
        ];

        let o_x_derivative = (o_x_errors[1] - o_x_errors[0]) / h;
        let o_y_derivative = (o_y_errors[1] - o_y_errors[0]) / h;
        let offset_step = Vector2::new(o_x_derivative, o_y_derivative);
        let d_x_derivative = (d_x_errors[1] - d_x_errors[0]) / h;
        let d_y_derivative = (d_y_errors[1] - d_y_errors[0]) / h;
        let direction_step = Vector2::new(d_x_derivative, d_y_derivative);
        l.offset -= offset_step * step_size;
        l.direction -= direction_step * step_size;
    }

    fn apply_grad_error_c(
        c1: &mut Circle,
        e: &FundamentalEntity,
        c: ConstraintType,
        step_size: f64,
    ) {
        let h = 1e-6;
        let x_errors = [
            Self::error(
                &FundamentalEntity::Circle(Circle {
                    pos: c1.pos + Vector2::new(-h / 2.0, 0.0),
                    radius: c1.radius,
                }),
                e,
                &c,
            ),
            Self::error(
                &FundamentalEntity::Circle(Circle {
                    pos: c1.pos + Vector2::new(h / 2.0, 0.0),
                    radius: c1.radius,
                }),
                e,
                &c,
            ),
        ];
        let y_errors = [
            Self::error(
                &FundamentalEntity::Circle(Circle {
                    pos: c1.pos + Vector2::new(0.0, -h / 2.0),
                    radius: c1.radius,
                }),
                e,
                &c,
            ),
            Self::error(
                &FundamentalEntity::Circle(Circle {
                    pos: c1.pos + Vector2::new(0.0, h / 2.0),
                    radius: c1.radius,
                }),
                e,
                &c,
            ),
        ];
        let r_errors = [
            Self::error(
                &FundamentalEntity::Circle(Circle {
                    pos: c1.pos,
                    radius: c1.radius - h / 2.0,
                }),
                e,
                &c,
            ),
            Self::error(
                &FundamentalEntity::Circle(Circle {
                    pos: c1.pos,
                    radius: c1.radius + h / 2.0,
                }),
                e,
                &c,
            ),
        ];

        let x_derivative = (x_errors[1] - x_errors[0]) / h;
        let y_derivative = (y_errors[1] - y_errors[0]) / h;
        let r_derivative = (r_errors[1] - r_errors[0]) / h;
        let step = Vector2::new(x_derivative, y_derivative);
        c1.pos -= step * step_size;
        c1.radius -= r_derivative * step_size;
    }
}

#[derive(Debug, Deserialize, Serialize, Clone, Copy)]
pub enum GuidedEntity {
    Point {
        id: EntityId,
    },
    Line {
        id: EntityId,
    },
    Circle {
        id: EntityId,
    },
    CappedLine {
        start: EntityId,
        end: EntityId,
        line: EntityId,
    },
    ArcThreePoint {
        start: EntityId,
        middle: EntityId,
        end: EntityId,
        circle: EntityId,
    },
}

impl GuidedEntity {
    /// `mouse_pos` is in sketch space
    pub fn filter_selection_attempt(&self, state: &State, mouse_pos: Vector2<f64>) -> bool {
        let entity_reg = &state.sketch.fundamental_entities;

        match self {
            GuidedEntity::Point { id } => true,
            GuidedEntity::Line { id } => true,
            GuidedEntity::Circle { id } => true,
            GuidedEntity::CappedLine { start, end, line } => {
                if let (
                    Some(FundamentalEntity::Point(start)),
                    Some(FundamentalEntity::Point(end)),
                    Some(FundamentalEntity::Line(line)),
                ) = (
                    entity_reg.get(start),
                    entity_reg.get(end),
                    entity_reg.get(line),
                ) {
                    let angle = (end.pos - start.pos).angle(&Vector2::x());
                    let rot = Rotation2::new(-angle);
                    let start_pos = rot * start.pos;
                    let end_pos = rot * end.pos;
                    let mouse_pos = rot * mouse_pos;

                    mouse_pos.x >= start_pos.x && mouse_pos.x <= end_pos.x
                        || mouse_pos.x <= start_pos.x && mouse_pos.x >= end_pos.x
                } else {
                    false
                }
            }
            GuidedEntity::ArcThreePoint {
                start,
                middle,
                end,
                circle,
            } => {
                if let (
                    Some(FundamentalEntity::Point(start)),
                    Some(FundamentalEntity::Point(middle)),
                    Some(FundamentalEntity::Point(end)),
                    Some(FundamentalEntity::Circle(circle)),
                ) = (
                    entity_reg.get(start),
                    entity_reg.get(middle),
                    entity_reg.get(end),
                    entity_reg.get(circle),
                ) {
                    let tolerance = 5.0 * PI / 180.0;
                    let start_angle = vector_angle(start.pos - circle.pos);
                    let mut end_angle = vector_angle(end.pos - circle.pos);
                    let middle_angle = vector_angle(middle.pos - circle.pos);
                    let mouse_angle = vector_angle(mouse_pos - circle.pos);

                    if middle_angle < start_angle && end_angle > start_angle {
                        end_angle -= 2.0 * PI;
                    }
                    if middle_angle > start_angle && end_angle < start_angle {
                        end_angle += 2.0 * PI;
                    }
                    if middle_angle < end_angle && start_angle > end_angle {
                        end_angle += 2.0 * PI;
                    }
                    if middle_angle > end_angle && start_angle < end_angle {
                        end_angle -= 2.0 * PI;
                    }

                    let min_angle = start_angle.min(end_angle) - tolerance;
                    let max_angle = start_angle.max(end_angle) + tolerance;
                    mouse_angle >= min_angle && mouse_angle <= max_angle
                        || (mouse_angle + 2.0 * PI >= min_angle
                            && mouse_angle + 2.0 * PI <= max_angle)
                        || (mouse_angle - 2.0 * PI >= min_angle
                            && mouse_angle - 2.0 * PI <= max_angle)
                } else {
                    false
                }
            }
        }
    }

    pub fn refers_to(&self, other: EntityId) -> bool {
        match self {
            GuidedEntity::Point { id } => *id == other,
            GuidedEntity::Line { id } => *id == other,
            GuidedEntity::Circle { id } => *id == other,
            GuidedEntity::CappedLine { start, end, line } => {
                *start == other || *end == other || *line == other
            }
            GuidedEntity::ArcThreePoint {
                start,
                middle,
                end,
                circle,
            } => *start == other || *middle == other || *end == other || *circle == other,
        }
    }
}

#[cfg(test)]
mod tests {
    use nalgebra::Vector2;

    use super::*;

    #[test]
    fn contraint_possibility_matrix() {
        let point = FundamentalEntity::Point(Point {
            pos: Vector2::<f64>::zeros(),
        });
        let circle = FundamentalEntity::Circle(Circle {
            pos: Vector2::<f64>::zeros(),
            radius: 0.0,
        });
        let line = FundamentalEntity::Line(Line {
            offset: Vector2::<f64>::zeros(),
            direction: Vector2::<f64>::zeros(),
        });

        assert!(BiConstraint::possible(
            &point,
            &point,
            &ConstraintType::Coincident
        ));
        assert!(BiConstraint::possible(
            &point,
            &point,
            &ConstraintType::Horizontal
        ));
        assert!(BiConstraint::possible(
            &point,
            &point,
            &ConstraintType::Vertical
        ));
        assert!(!BiConstraint::possible(
            &point,
            &point,
            &ConstraintType::Tangent
        ));
        assert!(!BiConstraint::possible(
            &point,
            &point,
            &ConstraintType::Parallel
        ));
        assert!(!BiConstraint::possible(
            &point,
            &point,
            &ConstraintType::Perpendicular
        ));
        assert!(!BiConstraint::possible(
            &point,
            &point,
            &ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &point,
            &point,
            &ConstraintType::Distance { x: 0.0 }
        ));
        assert!(!BiConstraint::possible(
            &point,
            &point,
            &ConstraintType::Angle { x: 0.0 }
        ));
        // --
        assert!(BiConstraint::possible(
            &circle,
            &point,
            &ConstraintType::Coincident
        ));
        assert!(BiConstraint::possible(
            &circle,
            &point,
            &ConstraintType::Horizontal
        ));
        assert!(BiConstraint::possible(
            &circle,
            &point,
            &ConstraintType::Vertical
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &point,
            &ConstraintType::Tangent
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &point,
            &ConstraintType::Parallel
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &point,
            &ConstraintType::Perpendicular
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &point,
            &ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &circle,
            &point,
            &ConstraintType::Distance { x: 0.0 }
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &point,
            &ConstraintType::Angle { x: 0.0 }
        ));
        // --
        assert!(BiConstraint::possible(
            &circle,
            &circle,
            &ConstraintType::Coincident
        ));
        assert!(BiConstraint::possible(
            &circle,
            &circle,
            &ConstraintType::Horizontal
        ));
        assert!(BiConstraint::possible(
            &circle,
            &circle,
            &ConstraintType::Vertical
        ));
        assert!(BiConstraint::possible(
            &circle,
            &circle,
            &ConstraintType::Tangent
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &circle,
            &ConstraintType::Parallel
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &circle,
            &ConstraintType::Perpendicular
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &circle,
            &ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &circle,
            &circle,
            &ConstraintType::Distance { x: 0.0 }
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &circle,
            &ConstraintType::Angle { x: 0.0 }
        ));
        // --
        assert!(BiConstraint::possible(
            &point,
            &line,
            &ConstraintType::Coincident
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            &ConstraintType::Horizontal
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            &ConstraintType::Vertical
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            &ConstraintType::Tangent
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            &ConstraintType::Parallel
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            &ConstraintType::Perpendicular
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            &ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &point,
            &line,
            &ConstraintType::Distance { x: 0.0 }
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            &ConstraintType::Angle { x: 0.0 }
        ));
        // --
        assert!(BiConstraint::possible(
            &circle,
            &line,
            &ConstraintType::Coincident
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            &ConstraintType::Horizontal
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            &ConstraintType::Vertical
        ));
        assert!(BiConstraint::possible(
            &circle,
            &line,
            &ConstraintType::Tangent
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            &ConstraintType::Parallel
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            &ConstraintType::Perpendicular
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            &ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &circle,
            &line,
            &ConstraintType::Distance { x: 0.0 }
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            &ConstraintType::Angle { x: 0.0 }
        ));
        // --
        assert!(!BiConstraint::possible(
            &line,
            &line,
            &ConstraintType::Coincident
        ));
        assert!(!BiConstraint::possible(
            &line,
            &line,
            &ConstraintType::Horizontal
        ));
        assert!(!BiConstraint::possible(
            &line,
            &line,
            &ConstraintType::Vertical
        ));
        assert!(!BiConstraint::possible(
            &line,
            &line,
            &ConstraintType::Tangent
        ));
        assert!(BiConstraint::possible(
            &line,
            &line,
            &ConstraintType::Parallel
        ));
        assert!(BiConstraint::possible(
            &line,
            &line,
            &ConstraintType::Perpendicular
        ));
        assert!(BiConstraint::possible(
            &line,
            &line,
            &ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &line,
            &line,
            &ConstraintType::Distance { x: 0.0 }
        ));
        assert!(BiConstraint::possible(
            &line,
            &line,
            &ConstraintType::Angle { x: 0.0 }
        ));
    }
}

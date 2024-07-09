use nalgebra::Vector2;
use serde::{Deserialize, Serialize};

use crate::registry::RegId;

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

#[derive(Debug, Deserialize, Serialize)]
pub enum FundamentalEntity {
    Point {
        pos: Vector2<f64>,
    },
    Line {
        offset: Vector2<f64>,
        direction: Vector2<f64>,
    },
    Circle {
        pos: Vector2<f64>,
        radius: f64,
    },
}

#[derive(Debug, Deserialize, Serialize)]
pub enum ConstraintType {
    Coincident,
    Horizontal,
    Vertical,
    Tangent,
    Parallel,
    Perpendicular,
    Colinear,
    Distance { x: f64 },
    Angle { x: f64 },
}

#[derive(Debug, Deserialize, Serialize)]
pub struct BiConstraint {
    e1: EntityId,
    e2: EntityId,
    c: ConstraintType,
}

impl BiConstraint {
    pub fn possible(e1: &FundamentalEntity, e2: &FundamentalEntity, c: ConstraintType) -> bool {
        match (e1, e2) {
            (FundamentalEntity::Point { .. }, FundamentalEntity::Point { .. }) => match c {
                ConstraintType::Coincident
                | ConstraintType::Distance { .. }
                | ConstraintType::Vertical
                | ConstraintType::Horizontal => true,
                _ => false,
            },
            (FundamentalEntity::Point { .. }, FundamentalEntity::Line { .. }) => match c {
                ConstraintType::Coincident
                | ConstraintType::Distance { .. }
                | ConstraintType::Angle { .. } => true,
                _ => false,
            },
            (FundamentalEntity::Point { .. }, FundamentalEntity::Circle { .. }) => match c {
                ConstraintType::Coincident
                | ConstraintType::Distance { .. }
                | ConstraintType::Vertical
                | ConstraintType::Horizontal => true,
                _ => false,
            },
            (FundamentalEntity::Line { .. }, FundamentalEntity::Line { .. }) => match c {
                ConstraintType::Parallel
                | ConstraintType::Perpendicular
                | ConstraintType::Colinear
                | ConstraintType::Distance { .. }
                | ConstraintType::Angle { .. } => true,
                _ => false,
            },
            (
                FundamentalEntity::Circle { pos, radius },
                FundamentalEntity::Line { offset, direction },
            ) => match c {
                ConstraintType::Coincident
                | ConstraintType::Tangent
                | ConstraintType::Distance { .. } => true,
                _ => false,
            },
            (FundamentalEntity::Circle { .. }, FundamentalEntity::Circle { .. }) => match c {
                ConstraintType::Coincident
                | ConstraintType::Distance { .. }
                | ConstraintType::Tangent
                | ConstraintType::Vertical
                | ConstraintType::Horizontal => true,
                _ => false,
            },
            (_, _) => BiConstraint::possible(e2, e1, c),
        }
    }
}

#[cfg(test)]
mod tests {
    use nalgebra::Vector2;

    use super::{BiConstraint, ConstraintType, FundamentalEntity};

    #[test]
    fn test_contraint_possibility_matrix() {
        let point = FundamentalEntity::Point {
            pos: Vector2::<f64>::zeros(),
        };
        let circle = FundamentalEntity::Circle {
            pos: Vector2::<f64>::zeros(),
            radius: 0.0,
        };
        let line = FundamentalEntity::Line {
            offset: Vector2::<f64>::zeros(),
            direction: Vector2::<f64>::zeros(),
        };

        assert!(BiConstraint::possible(
            &point,
            &point,
            ConstraintType::Coincident
        ));
        assert!(BiConstraint::possible(
            &point,
            &point,
            ConstraintType::Horizontal
        ));
        assert!(BiConstraint::possible(
            &point,
            &point,
            ConstraintType::Vertical
        ));
        assert!(!BiConstraint::possible(
            &point,
            &point,
            ConstraintType::Tangent
        ));
        assert!(!BiConstraint::possible(
            &point,
            &point,
            ConstraintType::Parallel
        ));
        assert!(!BiConstraint::possible(
            &point,
            &point,
            ConstraintType::Perpendicular
        ));
        assert!(!BiConstraint::possible(
            &point,
            &point,
            ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &point,
            &point,
            ConstraintType::Distance { x: 0.0 }
        ));
        assert!(!BiConstraint::possible(
            &point,
            &point,
            ConstraintType::Angle { x: 0.0 }
        ));
        // --
        assert!(BiConstraint::possible(
            &circle,
            &point,
            ConstraintType::Coincident
        ));
        assert!(BiConstraint::possible(
            &circle,
            &point,
            ConstraintType::Horizontal
        ));
        assert!(BiConstraint::possible(
            &circle,
            &point,
            ConstraintType::Vertical
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &point,
            ConstraintType::Tangent
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &point,
            ConstraintType::Parallel
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &point,
            ConstraintType::Perpendicular
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &point,
            ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &circle,
            &point,
            ConstraintType::Distance { x: 0.0 }
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &point,
            ConstraintType::Angle { x: 0.0 }
        ));
        // --
        assert!(BiConstraint::possible(
            &circle,
            &circle,
            ConstraintType::Coincident
        ));
        assert!(BiConstraint::possible(
            &circle,
            &circle,
            ConstraintType::Horizontal
        ));
        assert!(BiConstraint::possible(
            &circle,
            &circle,
            ConstraintType::Vertical
        ));
        assert!(BiConstraint::possible(
            &circle,
            &circle,
            ConstraintType::Tangent
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &circle,
            ConstraintType::Parallel
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &circle,
            ConstraintType::Perpendicular
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &circle,
            ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &circle,
            &circle,
            ConstraintType::Distance { x: 0.0 }
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &circle,
            ConstraintType::Angle { x: 0.0 }
        ));
        // --
        assert!(BiConstraint::possible(
            &point,
            &line,
            ConstraintType::Coincident
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            ConstraintType::Horizontal
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            ConstraintType::Vertical
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            ConstraintType::Tangent
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            ConstraintType::Parallel
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            ConstraintType::Perpendicular
        ));
        assert!(!BiConstraint::possible(
            &point,
            &line,
            ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &point,
            &line,
            ConstraintType::Distance { x: 0.0 }
        ));
        assert!(BiConstraint::possible(
            &point,
            &line,
            ConstraintType::Angle { x: 0.0 }
        ));
        // --
        assert!(BiConstraint::possible(
            &circle,
            &line,
            ConstraintType::Coincident
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            ConstraintType::Horizontal
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            ConstraintType::Vertical
        ));
        assert!(BiConstraint::possible(
            &circle,
            &line,
            ConstraintType::Tangent
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            ConstraintType::Parallel
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            ConstraintType::Perpendicular
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &circle,
            &line,
            ConstraintType::Distance { x: 0.0 }
        ));
        assert!(!BiConstraint::possible(
            &circle,
            &line,
            ConstraintType::Angle { x: 0.0 }
        ));
        // --
        assert!(!BiConstraint::possible(
            &line,
            &line,
            ConstraintType::Coincident
        ));
        assert!(!BiConstraint::possible(
            &line,
            &line,
            ConstraintType::Horizontal
        ));
        assert!(!BiConstraint::possible(
            &line,
            &line,
            ConstraintType::Vertical
        ));
        assert!(!BiConstraint::possible(
            &line,
            &line,
            ConstraintType::Tangent
        ));
        assert!(BiConstraint::possible(
            &line,
            &line,
            ConstraintType::Parallel
        ));
        assert!(BiConstraint::possible(
            &line,
            &line,
            ConstraintType::Perpendicular
        ));
        assert!(BiConstraint::possible(
            &line,
            &line,
            ConstraintType::Colinear
        ));
        assert!(BiConstraint::possible(
            &line,
            &line,
            ConstraintType::Distance { x: 0.0 }
        ));
        assert!(BiConstraint::possible(
            &line,
            &line,
            ConstraintType::Angle { x: 0.0 }
        ));
    }
}

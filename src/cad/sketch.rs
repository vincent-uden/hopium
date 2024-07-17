use rand::Rng;
use serde::{Deserialize, Serialize};

use crate::registry::Registry;

use super::entity::{BiConstraint, EntityId, FundamentalEntity};

#[derive(Debug, Serialize, Deserialize)]
pub struct Sketch {
    name: String,
    pub fundamental_entities: Registry<EntityId, FundamentalEntity>,
    pub bi_constraints: Vec<BiConstraint>,
    step_size: f64,
}

impl Sketch {
    pub fn new(name: String) -> Self {
        Self {
            name,
            fundamental_entities: Registry::new(),
            bi_constraints: Vec::new(),
            step_size: 1e-1,
        }
    }

    pub fn error(&self) -> f64 {
        let mut sum = 0.0;
        for BiConstraint { e1, e2, c } in &self.bi_constraints {
            sum += BiConstraint::error(
                &self.fundamental_entities[*e1],
                &self.fundamental_entities[*e2],
                c,
            );
        }
        sum
    }

    pub fn sgd_step(&mut self) {
        let mut rng = rand::thread_rng();
        for BiConstraint { e1, e2, c } in &self.bi_constraints {
            let [fe1, fe2] = self.fundamental_entities.get_many_mut([e1, e2]).unwrap();
            if rng.gen_bool(0.5) {
                BiConstraint::apply_grad_error(fe1, fe2, c, self.step_size);
            } else {
                BiConstraint::apply_grad_error(fe2, fe1, c, self.step_size);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use nalgebra::Vector2;

    use crate::cad::entity::{Circle, ConstraintType, Line, Point};

    use super::*;

    #[cfg(test)]
    impl Drop for Sketch {
        fn drop(&mut self) {
            println!("Dropping");
            let mut file = std::fs::File::create(format!(
                "./test_sketches/{}.json",
                self.name.replace(" ", "_").to_lowercase()
            ))
            .unwrap();
            serde_json::to_writer_pretty(&mut file, &self).expect("Failed to write sketch to file");
        }
    }

    #[test]
    fn basic_error_setup() {
        let mut sketch = Sketch::new("Basic Error Setup".to_string());
        let e1 = sketch
            .fundamental_entities
            .insert(FundamentalEntity::Point(Point {
                pos: Vector2::new(0.0, 0.0),
            }));
        let e2 = sketch
            .fundamental_entities
            .insert(FundamentalEntity::Point(Point {
                pos: Vector2::new(1.0, 1.0),
            }));
        sketch.bi_constraints.push(BiConstraint {
            e1,
            e2,
            c: ConstraintType::Horizontal,
        });

        assert!(sketch.error() > 0.0, "The error should be larger than 0")
    }

    #[test]
    fn basic_grad_error_setup() {
        let mut sketch = Sketch::new("Basic Grad Error Setup".to_string());
        let e1 = sketch
            .fundamental_entities
            .insert(FundamentalEntity::Point(Point {
                pos: Vector2::new(0.0, 0.0),
            }));
        let e2 = sketch
            .fundamental_entities
            .insert(FundamentalEntity::Point(Point {
                pos: Vector2::new(1.0, 1.0),
            }));
        sketch.bi_constraints.push(BiConstraint {
            e1,
            e2,
            c: ConstraintType::Horizontal,
        });

        let initial_error = sketch.error();
        for _ in 0..20000 {
            sketch.sgd_step();
        }
        let final_error = sketch.error();
        assert!(
            final_error < initial_error,
            "The final error should be smaller than the intial error"
        );
        assert!(final_error < 1e-2, "final_error {}", final_error);
    }

    #[test]
    fn pythagorean_triplet() {
        let mut sketch = Sketch::new("Pythagorean Triplet".to_string());
        let e1 = sketch
            .fundamental_entities
            .insert(FundamentalEntity::Point(Point {
                pos: Vector2::new(0.0, 0.0),
            }));
        let e2 = sketch
            .fundamental_entities
            .insert(FundamentalEntity::Point(Point {
                pos: Vector2::new(1.0, 0.1),
            }));
        let e3 = sketch
            .fundamental_entities
            .insert(FundamentalEntity::Point(Point {
                pos: Vector2::new(0.1, 1.0),
            }));

        sketch.bi_constraints.push(BiConstraint {
            e1,
            e2,
            c: ConstraintType::Horizontal,
        });
        sketch.bi_constraints.push(BiConstraint {
            e1,
            e2: e3,
            c: ConstraintType::Vertical,
        });
        sketch.bi_constraints.push(BiConstraint {
            e1,
            e2,
            c: ConstraintType::Distance { x: 3.0 },
        });
        sketch.bi_constraints.push(BiConstraint {
            e1,
            e2: e3,
            c: ConstraintType::Distance { x: 4.0 },
        });

        for _ in 0..20000 {
            sketch.sgd_step();
        }

        if let FundamentalEntity::Point(top_corner) = &sketch.fundamental_entities[e3] {
            if let FundamentalEntity::Point(right_corner) = &sketch.fundamental_entities[e2] {
                let diff = (top_corner.pos - right_corner.pos).norm();
                assert!((diff - 5.0) < 1e-6);
            }
        }

        assert!(
            sketch.error() < 1e-6,
            "The error should be smaller than 1e-6"
        );
    }

    #[test]
    fn circle_line_tangent() {
        let mut sketch = Sketch::new("Circle Line Tangent".to_string());
        let e1 = sketch
            .fundamental_entities
            .insert(FundamentalEntity::Circle(Circle {
                pos: Vector2::new(0.0, 0.0),
                radius: 1.0,
            }));
        let e2 = sketch
            .fundamental_entities
            .insert(FundamentalEntity::Line(Line {
                offset: Vector2::new(0.0, 0.0),
                direction: Vector2::new(1.0, 1.0),
            }));
        sketch.bi_constraints.push(BiConstraint {
            e1,
            e2,
            c: ConstraintType::Tangent,
        });

        assert!(sketch.error() > 0.0, "The error should be larger than 0");
        for _ in 0..20000 {
            sketch.sgd_step();
        }
        println!("{:?}", sketch);

        assert!(
            sketch.error() < 1e-6,
            "The error should be smaller than 1e-6"
        );
    }

    fn run_sketch_test<T>(test: T)
    where
        T: FnOnce() + std::panic::UnwindSafe,
    {
        let result = std::panic::catch_unwind(test);

        assert!(result.is_ok())
    }
}

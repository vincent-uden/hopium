use std::collections::VecDeque;

use nalgebra::Vector2;
use serde::{Deserialize, Serialize};

use crate::{
    cad::entity::ConstraintType,
    modes::MousePress,
    rendering::{
        area::{AreaId, AreaType},
        boundary::BoundaryId,
    },
    ui::UiId,
};

#[derive(Debug, Serialize, Deserialize, Clone, Copy)]
pub enum Event {
    PopMode,
    StartRotate,
    StopRotate,
    SplitPaneHorizontally {
        mouse_pos: Vector2<f64>,
    },
    SplitPaneVertically {
        mouse_pos: Vector2<f64>,
    },
    CollapseBoundary {
        mouse_pos: Vector2<f64>,
    },
    BoundaryMoved {
        start_pos: Vector2<f64>,
        end_pos: Vector2<f64>,
        bdry_id: BoundaryId,
    },
    DumpShapes,
    ExitProgram,
    DumpLayout,
    UiEntered {
        id: UiId,
    },
    UiExited {
        id: UiId,
    },
    ChangeAreaType {
        id: AreaId,
        area_type: AreaType,
    },
    SketchClick {
        pos: Vector2<f64>,
        sketch_space_select_radius: f64,
        press: MousePress,
    },
    Constrain {
        constraint_type: ConstraintType,
    },
}

fn should_serialize_as_layout(event: &Event) -> bool {
    matches!(
        event,
        Event::SplitPaneHorizontally { .. }
            | Event::SplitPaneVertically { .. }
            | Event::CollapseBoundary { .. }
            | Event::BoundaryMoved { .. }
            | Event::ChangeAreaType { .. }
    )
}

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct EventQueue {
    queue: VecDeque<Event>,
    history: Vec<Event>,
    history_index: i64,
}

impl EventQueue {
    pub fn new() -> Self {
        Self {
            queue: VecDeque::new(),
            history: Vec::new(),
            history_index: -1,
        }
    }

    pub fn post_event(&mut self, event: Event) {
        self.queue.push_back(event);
        self.history.push(event);
    }

    pub fn reset_history_index(&mut self) {
        self.history_index = 0;
    }

    pub fn pop(&mut self) -> Option<Event> {
        self.queue.pop_front()
    }

    pub fn get_next_history_event(&mut self) -> Option<Event> {
        if self.history_index < self.history.len() as i64 {
            let out = self.history[self.history_index as usize];
            self.history_index += 1;
            Some(out)
        } else {
            None
        }
    }

    pub fn is_empty(&self) -> bool {
        self.queue.is_empty()
    }

    pub fn layout_copy(&self) -> Self {
        let mut out = self.clone();
        out.history.retain(should_serialize_as_layout);
        out
    }
}

use std::collections::VecDeque;

use nalgebra::Vector2;
use serde::{Deserialize, Serialize};

#[derive(Debug, Serialize, Deserialize, Clone, Copy)]
pub enum Event {
    DisableSketchMode,
    ToggleSketchMode,
    PopMode,
    TogglePointMode,
    ToggleLineMode,
    ToggleTLineMode,
    ToggleExtrudeMode,
    ToggleDimensionMode,
    StartRotate,
    StopRotate,
    SplitPaneHorizontally { mouse_pos: Vector2<f64> },
    SplitPaneVertically { mouse_pos: Vector2<f64> },
    CollapseBoundary { mouse_pos: Vector2<f64> },
    //SketchPlaneHit { x: f64, y: f64, z: f64, ray: Ray },
    SketchClick { x: f64, y: f64, zoom_scale: f64 },
    //SketchConstrain ( ConstraintType type, ),
    ConfirmDimension,
    DumpShapes,
    IncreaseZoom,
    DecreaseZoom,
    ExitProgram,
}

#[derive(Debug, Serialize, Deserialize)]
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
}

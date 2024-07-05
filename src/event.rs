use nalgebra::Vector2;
use serde::{Deserialize, Serialize};

#[derive(Debug, Serialize, Deserialize)]
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

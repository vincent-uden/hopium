use std::fs;

use log::debug;
use nalgebra::Vector2;
use raylib::{RaylibHandle, RaylibThread};

use crate::{
    cad::{entity::EntityId, sketch::Sketch},
    event::Event,
    modes::{
        arc_three_point::ArcThreePoint, capped_line::CappedLine, circle::CircleMode,
        command::CommandMode, global::GlobalMode, line::LineMode, point::PointMode,
        sketch::SketchMode, ModeId,
    },
    rendering::{
        boundary::BoundaryOrientation,
        renderer::{Renderer, AREA_MAP, BDRY_MAP},
    },
    ui::MouseEventHandler,
    APP_STATE, EVENT_QUEUE, IMAGES, MODE_STACK,
};

pub struct App<'a> {
    renderer: Renderer,
    rl: &'a mut RaylibHandle,
    t: &'a RaylibThread,
}

impl<'a> App<'a> {
    pub fn new(
        screen_w: i32,
        screen_h: i32,
        rl: &'a mut RaylibHandle,
        t: &'a RaylibThread,
    ) -> Self {
        App {
            renderer: Renderer::new(screen_w, screen_h, rl, t),
            rl,
            t,
        }
    }

    pub fn update(&mut self) {
        self.renderer.update(self.rl);
        self.renderer.draw(self.rl, self.t);

        {
            let mut ms = MODE_STACK.lock().unwrap();
            ms.update(self.rl);
        }

        let mut is_empty = false;
        while !is_empty {
            let maybe_event;
            {
                let mut eq = EVENT_QUEUE.lock().unwrap();
                maybe_event = eq.pop();
            }
            {
                let mut ms = MODE_STACK.lock().unwrap();
                match maybe_event {
                    Some(event) => {
                        debug!("Processing event {:?}", event);
                        ms.process_event(event);
                        self.process_event(event);
                    }
                    None => {
                        is_empty = true;
                    }
                }
            }
        }
        let mut state = APP_STATE.lock().unwrap();
        if state.solving {
            state.sketch.sgd_step();
        }
    }

    pub fn run(&mut self) {
        let global_mode = Box::new(GlobalMode::new());
        let sketch_mode = Box::new(SketchMode::new());
        let point_mode = Box::new(PointMode::new());
        let line_mode = Box::new(LineMode::new());
        let circle_mode = Box::new(CircleMode::new());
        let command_mode = Box::new(CommandMode::new());
        let capped_line_mode = Box::new(CappedLine::new());
        let arc_three_point_mode = Box::new(ArcThreePoint::new());
        {
            let mut ms = MODE_STACK.lock().unwrap();
            ms.all_modes.insert(ModeId::Global, global_mode);
            ms.all_modes.insert(ModeId::Sketch, sketch_mode);
            ms.all_modes.insert(ModeId::Point, point_mode);
            ms.all_modes.insert(ModeId::Line, line_mode);
            ms.all_modes.insert(ModeId::Circle, circle_mode);
            ms.all_modes.insert(ModeId::Command, command_mode);
            ms.all_modes.insert(ModeId::CappedLine, capped_line_mode);
            ms.all_modes
                .insert(ModeId::ArcThreePoint, arc_three_point_mode);
            ms.push(ModeId::Global);
        }

        let mut running = true;
        while running {
            self.update();
            {
                running = APP_STATE.lock().unwrap().running;
            }
        }
        // These need to be cleared manually before renderer.rl is dropped, otherwise we segfault
        AREA_MAP.with_borrow_mut(|area_map| area_map.clear());
        BDRY_MAP.with_borrow_mut(|bdry_map| bdry_map.clear());
        IMAGES.with_borrow_mut(|images| images.clear());

        let eq = EVENT_QUEUE.lock().unwrap();
        fs::write(
            "layout.json",
            serde_json::to_string(&eq.layout_copy()).unwrap(),
        )
        .unwrap();
    }

    pub fn process_event(&mut self, event: Event) {
        match event {
            Event::DumpLayout => {
                self.renderer.dump_layout();
            }
            Event::DumpShapes => {
                let state = APP_STATE.lock().unwrap();
                println!("{:#?}", state.sketch);
            }
            Event::SplitPaneHorizontally { mouse_pos } => {
                self.renderer
                    .split_area(mouse_pos, BoundaryOrientation::Vertical, self.rl, self.t)
            }
            Event::SplitPaneVertically { mouse_pos } => self.renderer.split_area(
                mouse_pos,
                BoundaryOrientation::Horizontal,
                self.rl,
                self.t,
            ),
            Event::CollapseBoundary { mouse_pos } => {
                self.renderer.collapse_boundary(mouse_pos, self.rl)
            }
            Event::BoundaryMoved {
                start_pos: _,
                end_pos,
                bdry_id,
            } => {
                self.renderer.move_boundary(end_pos, bdry_id);
                AREA_MAP.with_borrow_mut(|area_map| {
                    for _area in area_map.values_mut() {
                        // area.build(self.rl);
                        // TODO: Implement an update_bounds method
                    }
                });
            }
            Event::ChangeAreaType { id, area_type } => {
                AREA_MAP.with_borrow_mut(|area_map| {
                    let area = &mut area_map[id];
                    area.area_type = area_type;
                    area.build(self.rl);
                });
            }
            Event::OpenCommandPalette => {
                let mut state = APP_STATE.lock().unwrap();
                state.command_palette_open = true;
                state.command_palette_input.clear();
                state.command_palette_pending_event = None;
                let mut eq = EVENT_QUEUE.lock().unwrap();
                eq.post_event(Event::PushMode(ModeId::Command));
            }
            Event::ExitProgram => {
                let mut state = APP_STATE.lock().unwrap();
                state.running = false;
            }
            Event::SaveSketch => {
                let state = APP_STATE.lock().unwrap();
                let json = serde_json::to_string(&state.sketch).unwrap();
                println!("{}", json);
                fs::write("sketch.json", json).unwrap();
            }
            Event::LoadSketch => {
                let mut state = APP_STATE.lock().unwrap();
                let json = fs::read_to_string("sketch.json").unwrap();
                let sketch: Sketch = serde_json::from_str(&json).unwrap();
                state.sketch = sketch;
            }
            _ => self.renderer.process_event(event, Vector2::zeros()),
        }
    }
}

#[derive(Debug)]
pub struct State {
    pub running: bool,
    pub sketch: Sketch,
    pub solving: bool,
    // Command palette
    pub command_palette_open: bool,
    pub command_palette_input: String,
    pub command_palette_pending_event: Option<Event>,
    // Sketch viewer
    pub pending_clicks: Vec<Vector2<f64>>,
    pub selected: Vec<EntityId>,
}

impl State {
    pub fn new() -> Self {
        Self {
            running: true,
            sketch: Sketch::new("Untitled".to_string()),
            solving: false,
            command_palette_open: false,
            command_palette_input: "".to_string(),
            command_palette_pending_event: None,
            pending_clicks: Vec::new(),
            selected: Vec::new(),
        }
    }
}

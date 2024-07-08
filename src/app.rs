use raylib::{RaylibHandle, RaylibThread};

use crate::{
    app_state,
    event::Event,
    event_queue, mode_stack,
    modes::global::GlobalMode,
    rendering::{
        boundary::BoundaryOrientation,
        renderer::{Renderer, AREA_MAP, BDRY_MAP},
    },
};

#[derive(Debug)]
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
            let mut ms = mode_stack.lock().unwrap();
            ms.update(self.rl);
        }

        let mut is_empty = false;
        while !is_empty {
            let maybe_event;
            {
                let mut eq = event_queue.lock().unwrap();
                maybe_event = eq.pop();
            }
            {
                let mut ms = mode_stack.lock().unwrap();
                match maybe_event {
                    Some(event) => {
                        ms.process_event(event);
                        self.process_event(event);
                    }
                    None => {
                        is_empty = true;
                    }
                }
            }
        }
    }

    pub fn run(&mut self) {
        let global_mode = Box::new(GlobalMode::new());
        {
            let mut ms = mode_stack.lock().unwrap();
            ms.push(global_mode);
        }

        let mut running = true;
        while running {
            self.update();
            {
                running = app_state.lock().unwrap().running;
            }
        }
        // These need to be cleared manually before renderer.rl is dropped, otherwise we segfault
        AREA_MAP.with_borrow_mut(|area_map| area_map.clear());
        BDRY_MAP.with_borrow_mut(|bdry_map| bdry_map.clear());
    }

    pub fn process_event(&mut self, event: Event) {
        match event {
            Event::DumpLayout => {
                self.renderer.dump_layout();
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
            Event::CollapseBoundary { mouse_pos } => self.renderer.collapse_boundary(mouse_pos),
            _ => {}
        }
    }
}

#[derive(Debug)]
pub struct State {
    pub running: bool,
}

impl State {
    pub fn new() -> Self {
        Self { running: true }
    }
}

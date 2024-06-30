package state

import "../sketch"
import "core:fmt"
import "vendor:raylib"

State :: struct {
  running: bool,
  sketch:  sketch.Sketch,
}

update :: proc(s: ^State) {
  fmt.println("Updating", raylib.GetKeyPressed())
}

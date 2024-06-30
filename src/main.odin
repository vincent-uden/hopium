package main

import "core:fmt"
import "core:math/linalg"
import "sketch"
import "state"
import "vendor:raylib"

programState := state.State {
  running = true,
}

main :: proc() {
  fmt.println("Hello, world!")

  raylib.InitWindow(1600, 900, "Odin CAD")

  for !raylib.WindowShouldClose() && programState.running {
    state.update(&programState)
  }

  raylib.CloseWindow()
}

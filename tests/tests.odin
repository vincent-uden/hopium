package tests

import "core:testing"

import "../src/sketch"

@(test)
vertically_constrain_points :: proc(t: ^testing.T) {
  p1 := sketch.Point{{0.0, 0.0}}
  p2 := sketch.Point{{1.0, 1.0}}

  c := sketch.Vertical{}

  testing.expect(t, sketch.error(p1, p2, c) > 0.0)

  // Carefully selected learning rate to converge in one step
  sketch.grad_error(&p1, &p2, c, 0.25)

  testing.expect_value(t, sketch.error(p1, p2, c), 0.0)
}

@(test)
horizontally_constrain_points :: proc(t: ^testing.T) {
  p1 := sketch.Point{{0.0, 0.0}}
  p2 := sketch.Point{{1.0, 1.0}}

  c := sketch.Horizontal{}

  testing.expect(t, sketch.error(p1, p2, c) > 0.0)

  // Carefully selected learning rate to converge in one step
  sketch.grad_error(&p1, &p2, c, 0.25)

  testing.expect_value(t, sketch.error(p1, p2, c), 0.0)
}

@(test)
coincidentally_constrain_points :: proc(t: ^testing.T) {
  p1 := sketch.Point{{0.0, 0.0}}
  p2 := sketch.Point{{1.0, 1.0}}

  c := sketch.Coincident{}

  testing.expect(t, sketch.error(p1, p2, c) > 0.0)

  for i := 0; i < 100; i += 1 {
    sketch.grad_error(&p1, &p2, c, 0.1)
  }

  error := sketch.error(p1, p2, c)
  testing.expectf(t, error < 1e-5, "Error: %f", error)
}

@(test)
distance_constrain_points :: proc(t: ^testing.T) {
  p1 := sketch.Point{{0.0, 0.0}}
  p2 := sketch.Point{{1.0, 1.0}}

  c := sketch.Distance{1.0}

  testing.expect(t, sketch.error(p1, p2, c) > 0.0)

  for i := 0; i < 100; i += 1 {
    sketch.grad_error(&p1, &p2, c, 0.1)
  }

  error := sketch.error(p1, p2, c)
  testing.expectf(t, error < 1e-5, "Error: %f", error)
}

@(test)
distance_constrain_point_and_line :: proc(t: ^testing.T) {
  p := sketch.Point{{0.0, 1.0}}
  l := sketch.Line{{0.0, 0.0}, {1.0, 0.0}}

  c := sketch.Distance{2.0}

  testing.expect(t, sketch.error(p, l, c) > 0.0)

  for i := 0; i < 1000; i += 1 {
    sketch.grad_error(&p, &l, c, 0.01)
  }

  error := sketch.error(p, l, c)
  testing.expectf(t, error < 1e-5, "Error: %f", error)
}

@(test)
proc_selection :: proc(t: ^testing.T) {
  s := sketch.Sketch{}

  p1 := sketch.Point{{0.0, 0.0}}
  p2 := sketch.Line{{1.0, 1.0}, {}}

  c := sketch.Distance{1.0}

  append(&s.entities, p1)
  append(&s.entities, p2)
  append(
    &s.constraints,
    sketch.GraphConstraint{c, &s.entities[0], &s.entities[1]},
  )

  constraint := s.constraints[0]
  sketch.error(constraint.e1, constraint.e2, constraint.c)
}

package sketch

import "core:fmt"
import "core:math"
import "core:math/linalg"

Point :: struct {
  pos: linalg.Vector2f64,
}

Line :: struct {
  offset:    linalg.Vector2f64,
  direction: linalg.Vector2f64,
}

GeometricEntity :: union #no_nil {
  Point,
  Line,
}

Coincident :: struct {}
Distance :: struct {
  dist: f64,
}
Horizontal :: struct {}
Vertical :: struct {}

Constraint :: union #no_nil {
  Coincident,
  Distance,
  Horizontal,
  Vertical,
}

error :: proc {
  error_pp,
  error_pl,
  error_lp,
  error_ll,
  error_ee,
}

error_pp :: proc(p1: Point, p2: Point, c: Constraint) -> f64 {
  using linalg.Vector2_Components

  switch constraint in c {
  case Coincident:
    return linalg.length2(p1.pos - p2.pos)
  case Distance:
    return(
      linalg.length2(p1.pos - p2.pos) -
      (constraint.dist * constraint.dist) \
    )
  case Horizontal:
    return math.pow(p1.pos[y] - p2.pos[y], 2.0)
  case Vertical:
    return math.pow(p1.pos[x] - p2.pos[x], 2.0)
  }
  return 0.0
}

error_pl :: proc(p1: Point, l2: Line, c: Constraint) -> f64 {
  using linalg
  orthoA := p1.pos - projection(p1.pos, l2.direction)
  orthoR := l2.offset - projection(l2.offset, l2.direction)

  fmt.println("Error pl")

  #partial switch constraint in c {
  case Coincident:
    return length2(orthoA + orthoR)
  case Distance:
    return math.pow(
      length2(orthoA + orthoR) - math.pow(constraint.dist, 2.0),
      2.0,
    )
  }
  return 0.0
}

error_lp :: proc(l1: Line, p2: Point, c: Constraint) -> f64 {
  return error_pl(p2, l1, c)
}

error_ll :: proc(l1: Line, l2: Line, c: Constraint) -> f64 {
  // TODO: Implement
  #partial switch constraint in c {
  }
  return 0.0
}

error_ee :: proc(
  e1: ^GeometricEntity,
  e2: ^GeometricEntity,
  c: Constraint,
) -> f64 {
  // This is psychotic. Is there a better way to do this down cast? Perhaps using type id?
  // We can move to a LUT approach when the switch becomes untennable as proposed by gingerBill
  //
  // Geometric_Entity_Kind :: enum {Point, Line}
  // to_kind :: proc(e: Geometric_Entity) -> Geometric_Entity_Kind {
  //  return ...
  // }
  // error_lut := [Geometric_Entity_Kind][Geometric_Entity_Kind]Error_Callback{...}
  switch e1 in e1^ {
  case Point:
    switch e2 in e2^ {
    case Point:
      return error(e1, e2, c)
    case Line:
      return error(e1, e2, c)
    }
  case Line:
    switch e2 in e2^ {
    case Point:
      return error(e1, e2, c)
    case Line:
      return error(e1, e2, c)
    }
  }

  return 0.0
}

grad_error :: proc {
  grad_error_pp,
  grad_error_pl,
  grad_error_lp,
  grad_error_ll,
  grad_error_ee,
}

grad_error_pp :: proc(p1: ^Point, p2: ^Point, c: Constraint, lr: f64) {
  using linalg
  using linalg.Vector2_Components
  dPos1 := Vector2f64{}
  dPos2 := Vector2f64{}

  #partial switch constraint in c {
  case Coincident:
    dPos1 = p1.pos - p2.pos
  case Distance:
    dist := length(p1.pos - p2.pos)
    dPos1 = (p1.pos - p2.pos) * (dist - constraint.dist) / dist
  case Vertical:
    dPos1[x] = 2 * (p1.pos[x] - p2.pos[x])
  case Horizontal:
    dPos1[y] = 2 * (p1.pos[y] - p2.pos[y])
  }

  dPos2 = -dPos1

  p1.pos -= dPos1 * lr
  p2.pos -= dPos2 * lr
}

grad_error_pl :: proc(p1: ^Point, l2: ^Line, c: Constraint, lr: f64) {
  using linalg

  orthoA := p1.pos - projection(p1.pos, l2.direction)
  orthoR := l2.offset - projection(l2.offset, l2.direction)

  // TODO: Is it possible to adjust the direction of the line?
  dPos := Vector2f64{}
  dDirection := Vector2f64{}
  dOffset := Vector2f64{}

  #partial switch constraint in c {
  case Coincident:
    dPos = (orthoA + orthoR)
  case Distance:
    dist := length(orthoA + orthoR)
    dPos = (orthoA + orthoR) * (dist - constraint.dist) / dist
  }

  dOffset = -dDirection

  p1.pos -= dPos * lr
  l2.offset -= dOffset * lr
}

grad_error_lp :: proc(l1: ^Line, p2: ^Point, c: Constraint, lr: f64) {
  grad_error_pl(p2, l1, c, lr)
}

grad_error_ll :: proc(l1: ^Line, l2: ^Line, c: Constraint, lr: f64) {
  // TODO:
}

grad_error_ee :: proc(
  e1: ^GeometricEntity,
  e2: ^GeometricEntity,
  c: Constraint,
  lr: f64,
) {
  switch &e1_ in e1 {
  case Point:
    switch &e2_ in e2 {
    case Point:
      grad_error(&(e1.(Point)), &(e2.(Point)), c, lr)
    case Line:
      grad_error(&(e1.(Point)), &(e2.(Line)), c, lr)
    }
  case Line:
    switch e2_ in e2 {
    case Point:
      grad_error(&(e1.(Line)), &(e2.(Point)), c, lr)
    case Line:
      grad_error(&(e1.(Line)), &(e2.(Line)), c, lr)
    }
  }
}

GraphConstraint :: struct {
  c:  Constraint,
  e1: ^GeometricEntity,
  e2: ^GeometricEntity,
}

Sketch :: struct {
  entities:    [dynamic]GeometricEntity,
  constraints: [dynamic]GraphConstraint,
}

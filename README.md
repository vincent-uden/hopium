# README

## Pacman packages
- raylib

## Roadmap
- Guided objects
    - Trimmed lines
    - Circle arcs
    - Fix drawing selection
- Fillets
- 3D View of sketch
- Extrusion

### Guided Objects
Only draw guided objects, make dummy guided objects for the 3 fundamental entities. Leading to N + 3 guided guided objects.

#### Capped line
A line starting and ending at a point. Two capped lines can share end points, which is determined at time of placement. Query for an existing point, use it if within a tolerance.

#### Adding a new object
- Add the guided object to the `GuidedEntity` enum
- Add a drawing method to `SketchViewer`
- Add pending drawing of unconfirmed object to `SketchViewer`
- Add a Mode for placing the guided object
- Add a ModeId for the mode
- Add some way of activating the mode (such as the command palette)
- Add the Mode to the `ModeStack` (in `App::run`)

# README

## Pacman packages
- raylib

## Roadmap
- Fillets
- 3D View of sketch
- Extrusion
- Rectangle selection

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

#### How does selection work?
Since you can't operate on the guided entities, their selection should not be tracked directly. Instead their color can be decided if their associated fundamental entities are selected.

However, this leaves the problem of how to select the fundamental entities. If one were to just go by their distance to the mouse point, they could be selected in areas where they are not visible in the guided entity. This would probably be quite confusing.

The least confusing solution is to define a distance to mouse point for every part of a guided entity, which then leads to the selection of it's underlying fundamental entity. This is a lot of work though. It might be necessary though as there's no general way to know what a guided entity will look like from it's constituent, fundamental parts.

One option which could save on some effort is to use the selection logic of the fundamental entities, then the guided entity gets a chance to filter out the possible selection.

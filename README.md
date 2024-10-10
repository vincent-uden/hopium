# README

## Pacman packages
- raylib

## Roadmap
- Guided objects
    - Trimmed lines
    - Circle arcs
- Fillets
- 3D View of sketch
- Extrusion

### Guided Objects
Only draw guided objects, make dummy guided objects for the 3 fundamental entities. Leading to N + 3 guided guided objects.

#### Capped line
A line starting and ending at a point. Two capped lines can share end points, which is determined at time of placement. Query for an existing point, use it if within a tolerance.

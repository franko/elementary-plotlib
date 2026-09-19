# Transformable Groups

This document describes the group feature built on the existing
[internal drawable model](drawables.md).

## Goals

A group is a reusable collection of independently styled drawing operations.
Its children have positions and geometric dimensions in a local coordinate
system. The group can be placed into a plot using a custom affine transform,
without reconstructing each child in plot coordinates.

The requirements are:

- Preserve each child's fill, stroke, flags, and painter order.
- Transform child positions and path geometry, but keep font sizes, marker sizes,
  stroke widths, and dash lengths screen-sized.
- Support data-space groups that follow the plot's coordinate scaling.
- Support screen-sized annotation groups anchored at a data coordinate.
- Allow nested groups and reuse of the same group at different placements.

Insertion uses copy/move-on-insertion snapshot semantics. Whether inserted
groups should later support live editing remains undecided; this feature does
not provide that capability.

## Existing Foundation

The architectural prerequisites were already available:

| Component | Responsibility |
| --- | --- |
| `elem_object` | Geometry, vertex generation, and specialized SVG output |
| `sg_drawable` | Cloning, autoscale bounds, and drawing with painted bounds |
| `sg_element` | Owned geometry, complete style, and local affine placement |
| `sg_drawable_list` | Ordered owning storage with deep-copy semantics |
| `graphics::plot` | Drawable roots in layers and the pending drawing queue |
| `virtual_canvas` | Existing backend-independent drawing interface |

Groups live above geometry, as another `sg_drawable`. A group does not inherit
from `elem_object` or the public `Object`: a single vertex stream cannot
represent independently colored and styled children naturally.

The hierarchy is kept internally rather than flattened at insertion. The group
owns its children, common transform, and aggregate bounds in one place. Plot
layers remain ordered lists of drawable roots, not a new scene-management
system.

## Coordinate Semantics

Let `P` be the current data-to-logical-screen transform, `T` the accumulated
local placement, and `p` a point in group-local coordinates. Logical screen
units are independent of physical HiDPI pixels; the canvas applies device scaling
afterward. Positive local Y follows the library's existing upward convention.

### Data-Space Groups

```text
screen_position = P(T(p))
```

The group is part of the plotted data. It contributes transformed data bounds
to automatic limits, and its geometry changes screen size when the plot limits
or viewport change.

The plot scales X and Y independently. Consequently, a local circle or rotated
shape is not necessarily shape-preserving on screen. This is normal data-space
behavior, not a group-specific aspect-ratio guarantee.

### Screen-Space Annotations

For an anchor `a` in data coordinates:

```text
screen_position = P(a) + T(p)
```

Only the anchor passes through the plot's data transform. Local positions,
path dimensions, and placement offsets use logical screen units. Changing plot
limits or window size moves the anchor but does not rescale the local layout.

For example, a unit-length local arrow placed with a scale of 24 remains 24
logical pixels long, regardless of the range of the data axes.

### Sizes and Orientation

Both modes preserve existing leaf rendering semantics:

- Path geometry follows the complete effective affine.
- Strokes and dash patterns are constructed after geometric transformation, so
  their widths and lengths remain screen-sized.
- Marker locations transform, but marker symbol sizes and orientations remain
  independent of the group transform.
- Text anchors transform, but glyph sizes and orientations remain independent
  of the group transform. `Text::SetAngle` still controls text rotation explicitly.

These rules describe the standard public primitives. Custom internal geometry
pipelines retain their own transform behavior, including any model-space
stroking they deliberately implement.

### Nesting and Composition

Within a group, every child uses its parent's coordinate frame. The root insertion
chooses data-space or annotation placement for the entire hierarchy. Mixed
data/screen coordinate boundaries inside one group are outside this feature.

For a child transform `C` inside a parent transform `T`, points are evaluated as
`P(T(C(p)))` for data placement.

With AGG's sequential multiplication convention:

```cpp
agg::trans_affine local_to_screen = local_to_parent;
local_to_screen.multiply(parent_to_screen);
```

This applies the local transform first. The older `trans_affine_compose` helper
is not used for group composition.

## Internal Implementation

### Group Drawable

`sg_group : public sg_drawable`, in `include/sg_group.h`, contains an
`sg_drawable_list` and a local-to-parent affine.

```text
sg_group
    local_to_parent
    ordered children: owned sg_drawable instances
```

Its three drawable operations:

1. `clone()` deep-copies the child list and copies the transform. Existing list
   ownership handles cleanup if a child clone throws.
2. `data_bounds()` obtains each child's bounds in group-local coordinates,
   transforms all four corners through the group's local-to-parent affine, and
   unions the defined results. An empty group returns no bounds.
3. `draw()` composes the group transform with the supplied parent-to-screen
   matrix and draws children in insertion order. When painted bounds are
   requested, it clears the output, obtains each child's painted bounds, and
   unions them. Those child bounds are already in screen coordinates and are
   not transformed again.

Transforming axis-aligned child bounding boxes is conservative, not necessarily
tight. Nested rotations can enlarge those bounds further. This is accepted;
tighter geometry-aware bounds would be a separate optimization.

A group starts with an identity root transform. `add_group()` copies or moves the
subgroup and applies its insertion placement once, through `place()`. Plot
insertion likewise applies the outer placement to the inserted root, not to
both the root and its children. If the root already has a transform, `place()`
composes its existing transform first and the insertion transform second.

Non-finite transforms are rejected by `check_finite_affine`, at construction and
at placement. Finite singular transforms are supported without inversion, so
geometry may collapse to lines or points.

### Annotation Placement

`sg_annotation` is a small root-only drawable wrapper for data-anchored screen
placement. It owns the group and a data-space anchor; the group retains its
local affine.

At draw time, the wrapper maps the anchor through the incoming plot transform
and passes a pure screen translation to the child group:

```cpp
double x = anchor.x;
double y = anchor.y;
parent_to_screen.transform(&x, &y);
group.draw(canvas, agg::trans_affine_translation(x, y), painted_bounds);
```

The group's own transform then supplies `T` in `P(a) + T(p)`. This deliberately
does not pass the plot's linear scaling into the group's layout. The anchor is
recomputed on every draw; screen coordinates are never cached during insertion.
An anchor whose image is not finite draws nothing.

The wrapper's `data_bounds()` returns no bounds. Its `clone()` independently
copies the child and anchor. This placement is restricted to plot-root
insertion, where the incoming matrix is known to map data to screen coordinates.
The existing drawable interface is sufficient; a more general render context
is unnecessary for these two placement modes.

### Bounds Policy

Data-space groups contribute geometry bounds, following existing text/marker
anchor-based autoscaling. Screen-sized stroke, glyph, and marker extents do not
expand data limits.

Annotations contribute neither their screen rectangle nor their anchor to
automatic limits. Otherwise, adding a label could unexpectedly change the
plotted view. Inferring data bounds from screen extents would also make limits
depend on the viewport and on the very transform they determine. Plots
containing only annotations should use explicit limits when the existing
fallback view is unsuitable. An optional anchor-only autoscale policy can be
considered later.

Both placement modes still contribute painted bounds to incremental redraws.
The existing distinction between unclipped logical-screen paint bounds and the
window update's rasterization padding is retained.

### Rendering and Clipping

Groups recurse through `virtual_canvas` and reuse `sg_element::draw`, which
preserves the specialized SVG dispatch for text, markers, dashes, and strokes.
Backend-native SVG `<g>` output is not required for correct grouping.

Groups inherit the plot's clipping behavior. Clipping is not reset when entering
or leaving a group, and annotations are not implicitly unclipped. Independent
group clipping and a clip stack are deferred. SVG clipping remains a backend
limitation and is not fixed by this feature.

### Curve Tessellation

`sg_element::draw` now derives the tessellation factor from the complete
effective affine. `approximation_scale_of` divides the largest singular value of
the linear part by `agg::trans_affine::scale()`, whose users multiply it back;
the product is therefore the maximum stretch rather than an average. The two
agree exactly for similarity transforms, so existing placements keep their
current approximation scale, while anisotropic scale and shear get a
conservative refinement instead of visibly under-tessellated curves.

## Public API

`Group` is a separate value type, alongside the `Affine` and `Point` value
types. AGG types stay out of installed public signatures.

`Affine` provides identity, translation, rotation, scaling, and general
six-coefficient construction for shear/reflection. Coefficients are
`x' = a*x + c*y + tx` and `y' = b*x + d*y + ty`, rotation angles are in radians,
and `Then(next)` applies this transform before `next`.

`Group` mirrors existing styled-object insertion with `Add` and `AddStroke`.
`Plot` uses the distinct names `AddGroup` and `AddAnnotation`; this also avoids
making existing member-function-pointer bindings for `Plot::Add` ambiguous.
`Group::AddGroup` supplies subgroup placement, while annotation insertion is
available only on `Plot`.

```cpp
using namespace elem;

Group symbol;
symbol.AddStroke(Path{{0, 0}, {1, 0}}, color::Blue, 1.5);

Text label{"A", 12};
label.SetPosition(1, 0);
symbol.Add(label, color::None, 0, color::Black);

Group assembly;
assembly.AddGroup(symbol, Affine::Rotation(0.4));

// Local geometry becomes data geometry. Styles stay screen-sized.
plot.AddGroup(
    assembly,
    Affine::Scale(2, 3).Then(Affine::Translation(10, 20)));

// Anchor in data; local layout and offset in logical pixels.
plot.AddAnnotation(
    symbol,
    Point{10, 20},
    Affine::Scale(24, 24).Then(Affine::Translation(8, 8)));
```

Default placement is identity. A group does not require an overall fill/stroke:
every leaf carries its own complete style. A public `Style` convenience type
remains optional.

For C, `elem_group` is an opaque handle with creation, deep-copy, free,
styled-child insertion, subgroup insertion, and plot-placement operations.
`elem_affine` and `elem_point` are plain structs, with `elem_affine_identity`,
`elem_affine_translation`, `elem_affine_rotation`, `elem_affine_scaling`, and
`elem_affine_then` as inline helpers. Group handles are distinct from
`elem_object*`; the geometry casts are not extended to pretend groups are vertex
sources. Ownership is documented on failure as well as on success, consistently
with the existing owning insertion path.

```c
elem_group *symbol = elem_group_new();
elem_path *line = elem_path_new();
elem_path_move_to(line, 0, 0);
elem_path_line_to(line, 1, 0);
elem_group_add(symbol, path_as_object(line), elem_color_blue, 1.5, elem_color_none, elem_stroke);

elem_plot_add_group(plot, symbol,
    elem_affine_then(elem_affine_scaling(2, 3), elem_affine_translation(10, 20)));
```

## Ownership and Insertion

C++ semantics match existing object insertion: passing an lvalue copies, passing
a moved value transfers ownership. Copies are independent, and editing a builder
after insertion does not change the plot's snapshot. There are no shared mutable
children or parent pointers; ownership by value prevents structural cycles. A
moved-from `Group` is inert rather than a crash: it copies as empty, and both
its own insertions and its insertion into a plot are no-ops.

The completed group or annotation wrapper is inserted as one drawable root,
under one plot lock and one window update/commit. Children are not looped
through the public `Plot::Add`, which would repeatedly update limits and expose
intermediate states. The group occupies one position in the layer's painter
order; all of its children draw consecutively at that position.

Layers continue to provide the existing removal and animation mechanism.
Changing or removing a particular inserted group through a handle is not part
of this contract. Live transforms would require a separate decision about
handles, synchronization, bounds invalidation, and redraw notification.

## Integration Points

The internal group header is `include/sg_group.h`, and the C handle is
`include/elem_group.h`. The shared four-corner bounds-transform helper, the
finite-affine check, and the approximation-scale helper live in
`include/sg_drawable.h`, where both the leaf and the group implementations use
them. The public headers are `include/elem/elem_c.h` and
`include/elem/elem_cpp.h`, implemented in `src/elem_c.cpp` and
`src/elem_cpp.cpp`. Plot storage and the canvas backends were not redesigned,
and the axis-only `sg_composite` is unchanged. Lua bindings are not updated.

## Validation

`tests/test-group.cpp` and `tests/test-public-group.cpp` extend the `core`
regression suite with:

- Mixed-style children, nested groups, and painter order across surrounding leaves.
- Translation, rotation, nonuniform scale, shear, reflection, and singular affines.
- Noncommuting composition order and repeated drawing without transform accumulation.
- Data bounds before/after rendering, empty groups, and changing automatic limits.
- Annotation-only plots and annotations that do not change existing data limits.
- Resize and limit changes: data geometry scales, annotation geometry does not.
- Screen-sized text, marker symbols, strokes, and dashes, including orientation rules.
- Deep copies, moves, reuse at different placements, source destruction, and clone failures.
- Full versus incremental rendering, painted bounds, inherited clipping, and HiDPI.
- Raster/SVG agreement, existing specialized SVG output, and C/C++ ownership behavior.

The public test compares a placed group against the same geometry placed by
hand, rendering both to SVG, so the placement is checked end to end rather than
against hard-coded layout coordinates.

```sh
meson compile -C .build
meson test -C .build --no-rebuild --suite core --print-errorlogs
```

`tests/test-plot-group.cpp` is a windowed example showing both placement modes.

The existing tests run unchanged as well: groups extend the drawable model
without regressing ordinary flat plotting.

## Deferred Features

This work does not depend on live scene editing, picking/events, shared nodes,
style inheritance, whole-group opacity, group-specific clipping, layout constraints,
or scaling the complete visual appearance including fonts and stroke widths.
The result is a small retained composition feature, not a general GUI scene graph.

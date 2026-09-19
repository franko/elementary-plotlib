# Internal Drawable Model

The public C and C++ geometry and plot APIs are unchanged. This internal layer
separates an owned drawing operation from the vertex source it renders. It does
not introduce groups, annotation placement, or mutable handles into a plot.

## Geometry and Drawing

- `elem_object` remains the geometry interface, including specialized SVG output.
- `sg_drawable` describes an independently owned drawing operation: `clone()`,
  `data_bounds()`, and `draw()`.
- `sg_element` is the styled leaf implementation. It owns one `elem_object`, its
  fill/stroke settings, and an identity-by-default local-to-parent affine.
- `sg_drawable_list` owns ordered drawables. Plots use it for their pending queue
  and layers; the existing axis-only `sg_composite` uses it for its children.

The geometry stays at a stable heap address because AGG converters can reference
other members of the same geometry object. Moving a drawable transfers ownership,
not the underlying converters. Copies deep-clone geometry and retain paint order.
A clone must return an independent owner or throw; unsupported borrowed adapters
fail explicitly rather than silently returning null or leaking a cloned source.

## Coordinates and Bounds

`draw()` receives a parent-to-logical-screen affine. Leaves compose their local
placement before that affine, so `screen_point = parent(local(point))`. Composition
uses AGG's sequential `multiply` convention rather than the older
`trans_affine_compose` helper. Every draw resolves placement anew, without baking
it into geometry. The canvas remains responsible for physical HiDPI scaling.

Stroke widths and dash lengths remain logical-screen sizes. Text and markers
continue to transform their anchors while retaining their own sizes/orientations.
The existing geometry adapters still control how their vertices are generated.

`data_bounds()` returns optional bounds in parent coordinates for autoscaling.
Leaves transform all four corners of their model bounding box. Empty geometry
returns no bounds. Text and markers retain their existing anchor-based model
bounds; pixel-sized decoration does not enlarge data limits.

The optional `painted_bounds` output of `draw()` is separate: it describes the
rendered geometry in logical-screen coordinates, before clipping. Fill and stroke
extents are unioned, including the actual generated stroke and its caps/joins.
Non-antialiased outlines receive a conservative one-unit pad. Invisible/empty
drawings return no painted bounds. Window updates still add rasterization padding.
Full redraws and pending-queue redraws use the same drawable rendering operation.

## Ownership and Compatibility

Insertion takes ownership. Pending entries move into the active layer, preserving
order without cloning. Clearing, popping, destruction, and failed deep copies
release all owners through RAII. Incremental commits retain geometric container
growth rather than reallocating for every added element.

The C entry point still consumes its object pointer. The C++ wrapper passes a
copy or moved value and releases it before the owning insertion call, including
when insertion throws. There is no change to installed headers or public class layouts.
Private-header users must rebuild for the new internal representation.

A future group can implement `sg_drawable` without becoming an `elem_object` or
changing layer storage. Coordinate-space policies and group APIs remain deferred.

## Regression Tests

The `core` Meson suite covers drawable placement/bounds, raster and SVG dispatch,
HiDPI, geometry cloning, pending/committed ownership, painter order, layers,
exception cleanup, and the existing public ownership API. It does not open windows:

```sh
meson compile -C .build
meson test -C .build --no-rebuild --suite core --print-errorlogs
```

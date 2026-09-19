# Group-Based Legend Implementation Memo

## Purpose

This memo proposes the next implementation step after the introduction of
transformable groups. It is based on the existing drawable model, the current
`Group::GetScreenBounds` measurement API, and the requested change in legend
semantics.

The legacy `SetLegendPlot` and `GetLegendPlot` APIs may be removed. The new
legend implementation should not treat a legend as a nested plot. A legend is
instead a list of independently built groups, with one group representing one
legend entry.

The immediate implementation prerequisite, measuring a group's actual painted
size in logical screen coordinates, is already available. This memo describes
how that facility should drive the complete legend design.

## Current Findings

The current legend implementation stores four nested `graphics::plot` objects
in `graphics::plot::m_legend`. Layout is computed from each child plot's model
bounding-box aspect ratio. Rendering recursively invokes child plot layout and
axis drawing. This creates several problems:

- Legend content is coupled to plot axes, limits, clipping, and titles.
- Text bounds are not measured as painted text. Existing text model bounds are
  anchor-only.
- Empty or degenerate legend bounds can produce invalid aspect-ratio arithmetic.
- Side legends are difficult to place without reserving space through the
  child-plot layout path.
- There is no plot-relative inside placement.
- `SetLegendPlot` does not follow the normal redraw and locking path.

Groups provide the correct lower-level representation. They already preserve
child style and painter order, support nested content, and use snapshot
ownership. A legend entry can therefore contain a line sample, marker sample,
text label, or any custom combination without introducing a second geometry
system.

## Design Goals

The implementation should satisfy these goals:

- Store legend entries as a list of groups rather than one monolithic group.
- Keep each entry independently measurable and independently positioned.
- Prefer a horizontal row for legends above and below the plot.
- Prefer a vertical column for legends left and right of the plot.
- Place inside legends relative to the actual active plot rectangle.
- Keep text, markers, stroke widths, and dash lengths screen-sized.
- Never use legend content for automatic data limits.
- Avoid scaling legend entries merely to fit available space.
- Reuse the existing drawable and affine composition rules.
- Make insertion snapshot-based and safe to copy, move, and reuse.
- Keep layout calculations deterministic across raster, SVG, resize, and HiDPI.

## Legend Entry Model

Each entry is a complete `Group`. The caller assembles the entry before adding
it to a legend:

```cpp
Group first;
first.AddStroke(Path{{0, 0}, {24, 0}}, color::Blue, 1.5);

Text first_label{"sin(x) / x", 14};
first_label.SetPosition(32, 0);
first.Add(first_label, color::Black, 0, color::Black, property::Fill);

Group second;
second.AddStroke(Path{{0, 0}, {24, 0}}, color::Red, 1.5);

Text second_label{"cos(x) / x", 14};
second_label.SetPosition(32, 0);
second.Add(second_label, color::Black, 0, color::Black, property::Fill);
```

The legend owns an ordered list of these entry groups. It does not flatten the
list at insertion time because layout needs each entry's independent bounds.
At draw time, each entry receives a translation into the final legend row or
column. The entry group remains reusable and its source can be used to build
another legend or another plot.

A small public `Legend` value type is preferable to exposing a raw
`std::vector<Group>` in every plot method. Its minimum responsibilities are:

- Own an ordered list of group entries.
- Add an entry using the same lvalue-copy and rvalue-move semantics as `Group`.
- Preserve entry order.
- Optionally carry gap and padding settings.

The internal representation can use `std::vector<sg_group>` or an owning list
of group roots. There is no need for parent pointers, shared nodes, or live
editing handles.

## Group Screen Measurement

The first implementation slice introduced these public types and operations:

```cpp
struct Bounds {
    double x1, y1, x2, y2;
    double Width() const;
    double Height() const;
};

bool Group::GetScreenBounds(Bounds& bounds,
                            Affine nominal = Affine{}) const;
```

The C equivalent is:

```c
typedef struct elem_bounds {
    double x1, y1, x2, y2;
} elem_bounds;

bool elem_group_get_screen_bounds(const elem_group *group,
                                  elem_affine nominal,
                                  elem_bounds *bounds);
```

The result is an unclipped rectangle in logical screen coordinates. `false`
means that the group is empty or contains no painted operation.

### Transform contract

The nominal affine is a parent-to-screen transform. The group's own
local-to-parent transform is composed before it:

```text
screen_position = nominal(group_transform(local_position))
```

This is the same composition order used by `sg_group::draw`. For a legend
entry, the normal measurement call is:

```cpp
Bounds entry_bounds;
entry.GetScreenBounds(entry_bounds, Affine::Identity());
```

The resulting width and height are intrinsic logical-screen dimensions. A
later translation positions the entry. The plot's data-to-screen matrix must
not be passed to normal legend measurement because legends are screen-space
content, not data geometry.

If a future legend feature applies a deliberate entry transform, that affine
can be passed as the nominal transform and the returned bounds describe the
final transformed screen rectangle.

### Measurement cost

The implementation uses `sg_measure_canvas`, a no-op `virtual_canvas`, and the
existing `painted_bounds` path. It resolves the same fill, stroke, dash,
marker, and glyph geometry used for drawing but does not allocate a raster
surface or perform rasterization.

Text currently follows the existing geometry-bound path. This is already much
cheaper than rendering to an image, while retaining accurate glyph extents.
If profiling later shows text measurement to be dominant, `graphics::text` can
provide a fast metric path based on glyph advance width and font height. That
optimization should remain an implementation detail and must preserve the
current screen-sized text semantics.

Because inserted legends are snapshots, intrinsic entry bounds can be cached
when the legend is installed. Resizing a window changes placement and wrapping
but does not require remeasuring unchanged entries.

## Legend API

The proposed C++ surface is:

```cpp
enum class LegendLocation {
    Right,
    Left,
    Bottom,
    Top,
    InsideTopLeft,
    InsideTopRight,
    InsideBottomLeft,
    InsideBottomRight,
    InsideTop,
    InsideBottom,
    InsideLeft,
    InsideRight,
};

class Legend {
public:
    Legend();
    void Add(Group entry);
};

void Plot::SetLegend(Legend legend, LegendLocation location);
void Plot::ClearLegend(LegendLocation location);
```

The exact padding and gap API can remain small initially. Defaults should be
internal constants, with explicit options added only if real callers need
them. The existing `Plot::Placement` enum can be removed if it has no other
purpose after the legacy APIs are deleted.

The C API should use an opaque `elem_legend` handle with creation, entry
insertion, free, and plot-placement functions. The handle should consume
`elem_group *` entries on insertion, consistently with the existing group
ownership rules.

The current C API has no text constructor. C legends with labels therefore
also require a minimal text factory, such as `elem_text_new` plus position and
angle setters, or a later C convenience API that creates standard legend
entries.

## Side Layout

Side layout should be based on measured entry rectangles rather than a child
plot's model aspect ratio.

For each entry, first obtain its intrinsic bounds. Normalize its content origin
by translating `x1` and `y1` to zero. The normalized entry size is:

```text
entry_width  = bounds.x2 - bounds.x1
entry_height = bounds.y2 - bounds.y1
```

For a top or bottom legend:

- Arrange entries from left to right.
- Use the sum of entry widths plus inter-entry gaps as the row width.
- Use the maximum entry height as the row height.
- Vertically center each entry within the row.
- Reserve row height plus legend padding outside the plot.

For a left or right legend:

- Arrange entries from top to bottom.
- Use the maximum entry width as the column width.
- Use the sum of entry heights plus inter-entry gaps as the column height.
- Align every entry to a common left edge within the column.
- Reserve column width plus legend padding outside the plot.

The layout must translate entries only. It should not nonuniformly scale a row
or column to fit the available canvas because that would produce undesirable
geometry scaling and could change the relationship between samples and text.

If the available width is insufficient for a top or bottom row, the preferred
fallback is wrapping into additional horizontal rows. Each row can use the
same measured entry sizes, with the legend height becoming the sum of row
heights plus row gaps. Font sizes and stroke widths remain unchanged. A later
iteration can add columns for side legends if vertical space is constrained.

Empty entries should be ignored for layout. An entirely empty legend should
reserve no space and should not enter any division or aspect-ratio calculation.

## Inside-Plot Layout

Inside legends should be positioned against `plot_layout.plot_active_area`,
not the outer `plot_area`. This keeps them aligned with the actual drawable
data region after axis labels and axis titles have consumed their margins.

The inside locations avoid exposing internal plot transforms. Corner locations
use a vertical column:

- `InsideTopLeft` aligns the legend's top-left bounds to the active area's
  top-left corner plus padding.
- `InsideTopRight` aligns its top-right bounds to the active area's top-right
  corner minus padding.
- `InsideBottomLeft` aligns its bottom-left bounds to the active area's
  bottom-left corner plus padding.
- `InsideBottomRight` aligns its bottom-right bounds to the active area's
  bottom-right corner minus padding.

The edge locations use the layout direction that matches their edge:

- `InsideTop` and `InsideBottom` arrange entries in a horizontal row, centered
  along the active area's width and aligned to the corresponding edge.
- `InsideLeft` and `InsideRight` arrange entries in a vertical column,
  vertically centered along the active area and aligned to the corresponding
  edge. Entries share a common left edge within the column.

Inside legends do not reserve layout space and do not contribute their anchor
or rectangle to automatic limits. They are plot-relative screen overlays.
Arbitrary data-anchored annotations remain the responsibility of
`Plot::AddAnnotation`.

Corner and left/right inside legends use a vertical column, which limits the
amount of plot area covered. The explicit top/bottom inside locations use a
horizontal row. Entries in a vertical column should share a common left edge.

## Rendering Order

The intended full-render order is:

1. Plot title.
2. Outside legends.
3. Axes and axis labels.
4. Ordinary plot layers.
5. Inside legends.

Outside legends should be clipped to their reserved rectangles. Inside legends
should follow the plot clipping policy and should normally be clipped to the
active plot area when clipping is enabled. The group itself should not create
or reset a new clipping stack.

The incremental drawing path needs explicit handling. Since inside legends
are painted after data, `draw_queue` must repaint them after pending data and
include their painted bounds in the damage rectangle. Otherwise newly added
data can cover an inside legend that was present in the saved plot image.

Changing a legend changes layout and therefore requires a full plot redraw.

## Ownership And Updates

Legend insertion should follow group snapshot semantics:

- Passing an lvalue copies the entry or legend.
- Passing an rvalue transfers ownership.
- Editing a source group or legend after insertion has no effect.
- Copying a plot deep-copies legend entries.
- Moving a plot transfers legend ownership.
- Replacing a location destroys the previous snapshot.
- Clearing a location removes its entries and releases their geometry.

The setter must acquire `graphics::plot::drawing_context`, replace the legend
slot atomically, invalidate layout state, and request a window refresh. Legend
children must not be inserted through repeated public `Plot::Add` calls; the
complete legend should be installed as one logical update.

## C API

The C API should mirror the C++ value semantics with an opaque handle:

```c
elem_legend *elem_legend_new(void);
void elem_legend_free(elem_legend *legend);
void elem_legend_add_group(elem_legend *legend, elem_group *entry);
void elem_plot_set_legend(elem_plot *plot, elem_legend *legend, int location);
void elem_plot_clear_legend(elem_plot *plot, int location);
```

The plot consumes the legend handle on placement, including on failure. The
legend consumes each entry group on insertion. `elem_legend` should not be
cast to `elem_group`; it is a list and layout owner, not a drawable geometry
source.

The location constants should preserve the existing side ordering if those
constants are reused, then append the four inside corner locations followed by
the four inside edge locations. Since the old C legend API was never
implemented, this is an additive C interface even though the C++ legacy API is
being removed.

## Migration Plan

The breaking migration should be deliberate:

1. Remove `Plot::SetLegendPlot` and `Plot::GetLegendPlot` from the public C++
   header and implementation.
2. Remove `graphics::plot::m_legend` nested plot storage and the child-plot
   legend layout path.
3. Replace the four legacy legend matrices with grouped legend slot metadata
   and cached entry measurements.
4. Add the new `Legend` and placement APIs.
5. Replace the existing window examples with group-entry examples.
6. Remove or rewrite `test-plot-legend.cpp` and `test-plot-legend-2.cpp` as
   deterministic grouped-legend tests.
7. Update the plot and C manuals to describe grouped entries and inside
   placement.
8. Keep `AddAnnotation` unchanged for arbitrary data-coordinate annotations.

The first screen-bounds commit should remain a reusable foundation. It should
not be coupled to legend storage or legend layout code.

## Testing Plan

The core suite should cover the following independently from window tests:

- A measured group matches the painted bounds returned by an actual draw.
- Translation, rotation, nonuniform scale, shear, reflection, and singular
  nominal transforms use the documented composition order.
- Stroke, dash, marker, and text extents are included.
- Text and marker visual sizes remain screen-sized under nominal scaling.
- Empty and non-painted groups return no bounds.
- Repeated measurement does not accumulate transforms.
- C and C++ measurement results agree.
- Top and bottom entries form horizontal rows.
- Left and right entries form vertical columns.
- Long labels wrap without scaling glyphs or strokes.
- Empty entries and empty legends reserve no space.
- Side legends reserve measured space without changing data limits.
- Inside legends follow active-area corners through resize and axis-margin
  changes.
- Inside legends remain above ordinary data in full and incremental rendering.
- Legend replacement, clearing, copying, moving, and source destruction are
  safe.
- Raster and SVG output preserve entry order and text content.
- HiDPI changes physical pixels but not logical measured bounds.

## Open Decisions

The following choices can remain explicit until the first implementation is
used:

- Whether `Legend` exposes configurable padding and gaps immediately or starts
  with internal defaults.
- Whether top and bottom wrapping is enabled in the first layout version or
  deferred until overflow cases are observed.
- Whether inside legends default to vertical layout or inherit the side-based
  direction rule.
- Whether a future arbitrary inside rectangle should be supported in addition
  to the four named corners.
- Whether C text construction is included in the first C legend milestone.
- Whether SVG clipping is upgraded as part of legend work or remains subject
  to the existing backend limitation.

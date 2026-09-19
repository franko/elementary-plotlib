#include "plot.h"
#include "sg_group.h"
#include "canvas_dashed.h"
#include "canvas_markers.h"
#include "canvas_path.h"
#include "canvas_svg.h"
#include "text.h"

#include <cmath>
#include <cstdio>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using graphics::plot;
using namespace graphics::property;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void same_rect(const agg::rect_d& a, const agg::rect_d& b, const char* message) {
    require(std::abs(a.x1 - b.x1) < 1e-8 && std::abs(a.y1 - b.y1) < 1e-8 &&
            std::abs(a.x2 - b.x2) < 1e-8 && std::abs(a.y2 - b.y2) < 1e-8, message);
}

void same_rect(const opt_rect<double>& bounds, const agg::rect_d& r, const char* message) {
    require(bounds.is_defined(), message);
    same_rect(bounds.rect(), r, message);
}

struct clone_failure {};
struct counts {
    int created = 0, destroyed = 0, copies = 0, fail_after = -1;
    int live() const { return created - destroyed; }
};

// A unit-ish box whose lifetime and cloning are observable.
struct counted_box : elem_path {
    counts& count;
    agg::rect_d box;
    counted_box(counts& c, const agg::rect_d& r) : count(c), box(r) {
        move_to(r.x1, r.y1); line_to(r.x2, r.y1);
        line_to(r.x2, r.y2); line_to(r.x1, r.y2); close_polygon();
        ++count.created;
    }
    ~counted_box() override { ++count.destroyed; }
    elem_object* copy() const override {
        if (count.fail_after == 0) throw clone_failure{};
        if (count.fail_after > 0) --count.fail_after;
        auto result = std::make_unique<counted_box>(count, box);
        result->m_scaling_matrix = m_scaling_matrix;
        ++count.copies;
        return result.release();
    }
};

agg::rgba8 ink(int id) { return agg::rgba8(id, 23, 45); }

std::unique_ptr<elem_path> box(const agg::rect_d& r) {
    auto path = std::make_unique<elem_path>();
    path->move_to(r.x1, r.y1); path->line_to(r.x2, r.y1);
    path->line_to(r.x2, r.y2); path->line_to(r.x1, r.y2); path->close_polygon();
    return path;
}

sg_element leaf(int id, const agg::rect_d& r = {0, 0, 1, 1}, unsigned flags = fill,
                const agg::trans_affine& local = {}) {
    return sg_element(box(r).release(), ink(id), 2, ink(id), flags, local);
}

sg_element counted_leaf(counts& c, int id, const agg::rect_d& r = {0, 0, 1, 1},
                        unsigned flags = fill, const agg::trans_affine& local = {}) {
    return sg_element(new counted_box(c, r), ink(id), 2, ink(id), flags, local);
}

std::unique_ptr<sg_group> group_of(std::initializer_list<int> ids,
                                   const agg::trans_affine& local = {}) {
    auto group = std::make_unique<sg_group>(local);
    for (int id : ids) group->add(leaf(id));
    return group;
}

struct recording_canvas : virtual_canvas {
    struct paint { int id, mode; agg::rect_d bounds; };
    std::vector<paint> paints;
    std::vector<agg::rect_i> clips;
    int resets = 0;
    opt_rect<double> painted, damage;
    plot_render_info info;

    void record(elem_object& object, agg::rgba8 color, int mode) {
        agg::rect_d r;
        require(agg::bounding_rect_single(object, 0, &r.x1, &r.y1, &r.x2, &r.y2),
                "painted geometry is empty");
        if (mode == 2) { r.x1 -= 1; r.y1 -= 1; r.x2 += 1; r.y2 += 1; }
        paints.push_back({color.r, mode, r});
        painted.add<rect_union>(r);
    }
    void draw(elem_object& o, agg::rgba8 c) override { record(o, c, 0); }
    void draw_noaa(elem_object& o, agg::rgba8 c) override { record(o, c, 1); }
    void draw_outline_noaa(elem_object& o, agg::rgba8 c) override { record(o, c, 2); }
    void clip_logical_area(const agg::rect_i& r) override { clips.push_back(r); }
    void reset_clipping() override { ++resets; }
};

recording_canvas render(sg_drawable& drawable, const agg::trans_affine& m,
                        bool want_bounds = true) {
    recording_canvas canvas;
    opt_rect<double> bounds;
    drawable.draw(canvas, m, want_bounds ? &bounds : nullptr);
    canvas.damage = bounds;
    return canvas;
}

const agg::trans_affine slot(640, 0, 0, 480, 13, 17);

recording_canvas render(plot& p, bool queue = false) {
    recording_canvas canvas;
    plot::drawing_context context(p);
    p.draw(context, canvas, slot, &canvas.info);
    if (queue) {
        canvas.paints.clear(); canvas.painted.clear();
        canvas.clips.clear(); canvas.resets = 0;
        p.draw_queue(context, canvas, slot, canvas.info, canvas.damage);
    }
    return canvas;
}

void order(const recording_canvas& canvas, std::initializer_list<int> ids,
           const char* message = "painter order changed") {
    require(canvas.paints.size() == ids.size(), "wrong paint count");
    size_t i = 0;
    for (int id : ids) require(canvas.paints[i++].id == id, message);
}

// Corners of a box through an affine, matching the conservative bounds policy.
agg::rect_d corners(const agg::rect_d& r, const agg::trans_affine& m) {
    opt_rect<double> result = transform_bounds(r, m);
    require(result.is_defined(), "expected bounds are undefined");
    return result.rect();
}

agg::trans_affine then(const agg::trans_affine& first, const agg::trans_affine& second) {
    agg::trans_affine m = first;
    m.multiply(second);
    return m;
}

// Unit-square coordinates of a plot with limits (0,0)-(10,10) on a slot matrix.
agg::rect_d in_plot(const agg::rect_d& data, const agg::trans_affine& area) {
    return agg::rect_d(area.tx + data.x1 / 10 * area.sx, area.ty + data.y1 / 10 * area.sy,
                       area.tx + data.x2 / 10 * area.sx, area.ty + data.y2 / 10 * area.sy);
}

void composition() {
    const agg::trans_affine parent(2, 0, 0, 3, 11, 13);
    const agg::trans_affine outer(0, 1, -1, 0, 5, 7);   // rotation by 90 degrees
    const agg::trans_affine inner(1, 0, 0.5, 1, 0, 0);  // shear
    const agg::rect_d unit(0, 0, 1, 1);

    sg_group group(outer);
    auto child = std::make_unique<sg_group>(inner);
    child->add(leaf(1, unit));
    group.add(std::move(child));
    group.add(leaf(2, unit));

    const auto painted = render(group, parent);
    order(painted, {1, 2});
    same_rect(painted.paints[0].bounds, corners(unit, then(then(inner, outer), parent)),
              "nested group composition applies the local transform last");
    same_rect(painted.paints[1].bounds, corners(unit, then(outer, parent)),
              "group transform missing from a direct child");

    // Repeated drawing must resolve placement anew, never accumulate it.
    const auto again = render(group, parent);
    same_rect(again.paints[0].bounds, painted.paints[0].bounds, "group transform accumulated");
    same_rect(again.paints[1].bounds, painted.paints[1].bounds, "group transform accumulated");

    // Noncommuting composition: rotate-then-translate differs from the reverse.
    const agg::trans_affine rotate(0, 1, -1, 0, 0, 0);
    const agg::trans_affine move = agg::trans_affine_translation(10, 0);
    sg_group a(rotate), b(move);
    a.place(move);
    b.place(rotate);
    a.add(leaf(1, unit)); b.add(leaf(1, unit));
    const auto rotated_first = render(a, agg::trans_affine());
    const auto moved_first = render(b, agg::trans_affine());
    same_rect(rotated_first.paints[0].bounds, corners(unit, then(rotate, move)),
              "place() composes the existing transform after the placement");
    same_rect(moved_first.paints[0].bounds, corners(unit, then(move, rotate)),
              "place() composes the existing transform after the placement");
    require(std::abs(rotated_first.paints[0].bounds.x1 - moved_first.paints[0].bounds.x1) > 1,
            "noncommuting transforms produced the same placement");

    // add_group applies its placement once, to the inserted root only.
    sg_group nested;
    sg_group reused(agg::trans_affine_scaling(2, 2));
    reused.add(leaf(1, unit));
    nested.add_group(reused, agg::trans_affine_translation(100, 0));
    nested.add_group(reused, agg::trans_affine_translation(0, 100));
    const auto placed = render(nested, agg::trans_affine());
    same_rect(placed.paints[0].bounds, {100, 0, 102, 2}, "subgroup placement applied twice");
    same_rect(placed.paints[1].bounds, {0, 100, 2, 102}, "reused subgroup shares a placement");
    require(reused.size() == 1, "add_group modified the source group");

    // A group draws its children consecutively, at one position in the order.
    sg_group mixed;
    mixed.add(leaf(1, unit));
    mixed.add(std::move(group_of({2, 3})));
    mixed.add(leaf(4, unit));
    order(render(mixed, agg::trans_affine()), {1, 2, 3, 4},
          "group children are not drawn consecutively");
}

void bounds() {
    const agg::rect_d unit(0, 0, 1, 1);
    const agg::trans_affine placement(2, 0, 0, 3, 10, 20);

    sg_group group(placement);
    group.add(leaf(1, {1, 2, 3, 4}));
    group.add(leaf(2, {-1, -1, 0, 0}));
    same_rect(group.data_bounds(), corners({-1, -1, 3, 4}, placement),
              "group bounds ignore the local placement");

    sg_group empty;
    require(!empty.data_bounds().is_defined(), "empty group contributes bounds");
    empty.add(std::make_unique<sg_group>());
    require(!empty.data_bounds().is_defined(), "empty subgroup contributes bounds");

    // Nested groups accumulate their transforms in the bounds as well.
    sg_group outer(agg::trans_affine_translation(100, 0));
    auto inner = std::make_unique<sg_group>(agg::trans_affine_scaling(2, 2));
    inner->add(leaf(1, unit));
    outer.add(std::move(inner));
    same_rect(outer.data_bounds(), {100, 0, 102, 2}, "nested group bounds lost a transform");

    // Reflection and shear: conservative axis-aligned bounds of the corners.
    sg_group reflected(agg::trans_affine(-2, 0.5, 1, 3, 10, -7));
    reflected.add(leaf(1, {-2, -1, 3, 4}));
    same_rect(reflected.data_bounds(), corners({-2, -1, 3, 4}, agg::trans_affine(-2, .5, 1, 3, 10, -7)),
              "reflected group bounds are wrong");

    // Finite singular transforms are supported: geometry collapses to a line.
    sg_group singular(agg::trans_affine(1, 0, 0, 0, 0, 5));
    singular.add(leaf(1, unit));
    same_rect(singular.data_bounds(), {0, 5, 1, 5}, "singular group bounds are wrong");
    const auto collapsed = render(singular, agg::trans_affine());
    same_rect(collapsed.paints[0].bounds, {0, 5, 1, 5}, "singular group did not draw");

    // Non-finite transforms are rejected, at construction and at placement.
    const double infinity = std::numeric_limits<double>::infinity();
    bool rejected = false;
    try { sg_group bad(agg::trans_affine(1, 0, 0, 1, infinity, 0)); }
    catch (const std::invalid_argument&) { rejected = true; }
    require(rejected, "non-finite group transform was accepted");
    rejected = false;
    try { group.place(agg::trans_affine(std::nan(""), 0, 0, 1, 0, 0)); }
    catch (const std::invalid_argument&) { rejected = true; }
    require(rejected, "non-finite placement was accepted");
    same_rect(group.data_bounds(), corners({-1, -1, 3, 4}, placement),
              "rejected placement changed the group");

    // Text and markers keep their anchor-based model bounds inside a group.
    sg_group anchors(agg::trans_affine_scaling(3, 5));
    auto text = std::make_unique<graphics::text>("A", 12);
    text->set_point(1, 2);
    anchors.add(sg_element(text.release(), colors::black(), 0, colors::black(), fill));
    auto markers = std::make_unique<elem_markers>(10, new_marker_symbol(2));
    markers->move_to(2, 3);
    anchors.add(sg_element(markers.release(), colors::zero(), 0, colors::black(), fill));
    same_rect(anchors.data_bounds(), {3, 10, 6, 15},
              "screen-sized decoration enlarged the group data bounds");
}

void screen_sized_styles() {
    const agg::trans_affine wide(40, 0, 0, 1, 0, 0);

    // Stroke widths follow the leaf, not the group transform.
    auto line = std::make_unique<elem_path>();
    line->move_to(0, 0); line->line_to(1, 0);
    sg_group strokes(wide);
    strokes.add(sg_element(line.release(), colors::black(), 8, colors::zero(), stroke));
    const auto painted = render(strokes, agg::trans_affine());
    const auto& r = painted.paints[0].bounds;
    // Round caps are polygonal approximations with subpixel tolerance.
    require(std::abs(r.x1 + 4) < .125 && std::abs(r.x2 - 44) < .125 &&
            std::abs(r.y1 + 4) < .125 && std::abs(r.y2 - 4) < .125,
            "stroke width scaled with the group transform");

    // Marker symbol sizes and orientations are independent of the group.
    auto markers = std::make_unique<elem_markers>(10, new_marker_symbol(2));
    markers->move_to(1, 1);
    sg_group symbols;
    symbols.add(sg_element(markers.release(), colors::zero(), 0, colors::black(), fill));
    const auto plain = render(symbols, agg::trans_affine());
    symbols.place(agg::trans_affine(0, 4, -7, 0, 0, 0)); // rotate and stretch
    const auto placed = render(symbols, agg::trans_affine());
    const auto& before = plain.paints[0].bounds;
    const auto& after = placed.paints[0].bounds;
    require(std::abs((before.x2 - before.x1) - (after.x2 - after.x1)) < 1e-8 &&
            std::abs((before.y2 - before.y1) - (after.y2 - after.y1)) < 1e-8,
            "marker symbol size or orientation followed the group transform");

    // Dash lengths are measured after the geometric transformation.
    auto dashes = std::make_unique<elem_dashed>();
    dashes->move_to(0, 0); dashes->line_to(1, 0);
    dashes->add_dash(4, 4);
    sg_group dashed(wide);
    dashed.add(sg_element(dashes.release(), colors::black(), 1, colors::zero(), stroke));
    const auto dash_paint = render(dashed, agg::trans_affine());
    // 40 screen units of line, cut into 4-unit dashes, reaches the far end.
    require(dash_paint.paints[0].bounds.x2 > 35, "dash lengths scaled with the group transform");
}

void annotations() {
    const agg::rect_d unit(0, 0, 1, 1);
    sg_group symbol(agg::trans_affine_scaling(24, 24));
    symbol.add(leaf(1, unit));

    sg_annotation annotation(symbol, 10, 20);
    require(!annotation.data_bounds().is_defined(), "annotation contributes data bounds");

    // Only the anchor passes through the incoming transform.
    const agg::trans_affine data_to_screen(2, 0, 0, 3, 100, 200);
    const auto painted = render(annotation, data_to_screen);
    same_rect(painted.paints[0].bounds, {120, 260, 144, 284},
              "annotation layout followed the data transform");
    same_rect(painted.damage, painted.painted.rect(), "annotation painted bounds are wrong");

    // A different data transform moves the anchor but keeps the local layout.
    const auto moved = render(annotation, agg::trans_affine(7, 0, 0, 11, -3, -5));
    same_rect(moved.paints[0].bounds, {67, 215, 91, 239}, "annotation layout rescaled");

    // Clones are independent, and a non-finite anchor is rejected.
    auto clone = annotation.clone();
    const auto cloned_paint = render(*clone, data_to_screen);
    same_rect(cloned_paint.paints[0].bounds, painted.paints[0].bounds, "annotation clone differs");
    require(!clone->data_bounds().is_defined(), "annotation clone contributes data bounds");
    bool rejected = false;
    try { sg_annotation bad(symbol, 0, std::nan("")); }
    catch (const std::invalid_argument&) { rejected = true; }
    require(rejected, "non-finite annotation anchor was accepted");

    // A non-finite anchor image draws nothing rather than corrupting the canvas.
    const auto degenerate = render(annotation, agg::trans_affine(
        std::numeric_limits<double>::infinity(), 0, 0, 1, 0, 0));
    require(degenerate.paints.empty() && !degenerate.damage.is_defined(),
            "annotation drew through a non-finite anchor");
}

void plot_integration() {
    {   // Painter order across surrounding leaves, in one layer position.
        plot p(0);
        p.set_limits({0, 0, 10, 10});
        p.add(leaf(1, {0, 0, 1, 1}));
        auto group = group_of({2, 3}, agg::trans_affine_translation(5, 5));
        p.add(std::move(group));
        p.add(leaf(4, {0, 0, 1, 1}));
        const auto committed = render(p, true);
        order(committed, {1, 2, 3, 4}, "group broke the plot painter order");
        const auto& area = committed.info.active_area;
        same_rect(committed.paints[1].bounds, in_plot({5, 5, 6, 6}, area),
                  "group placement lost the plot transform");
        // Painted bounds reach the incremental redraw unchanged.
        same_rect(committed.damage.rect(), committed.painted.rect(),
                  "group painted bounds are missing from the redraw damage");
        // Clipping is inherited: a group neither clips nor resets on its own.
        require(committed.resets == 1, "group changed the plot clipping discipline");

        // Full and incremental rendering agree.
        p.commit_pending_draw();
        const auto full = render(p);
        order(full, {1, 2, 3, 4}, "committed group lost its painter order");
        same_rect(full.paints[1].bounds, committed.paints[1].bounds,
                  "full and incremental group rendering disagree");
    }
    {   // Data-space groups follow the automatic limits; annotations do not.
        plot p(plot::auto_limits);
        agg::rect_d box;
        p.add(std::move(group_of({1}, agg::trans_affine(4, 0, 0, 6, 1, 2))));
        p.get_bounding_rect(box);
        same_rect(box, {1, 2, 5, 8}, "group did not contribute transformed data bounds");

        sg_group annotated(agg::trans_affine_scaling(1000, 1000));
        annotated.add(leaf(2, {0, 0, 1, 1}));
        p.add(std::make_unique<sg_annotation>(std::move(annotated), 500, 500));
        p.get_bounding_rect(box);
        same_rect(box, {1, 2, 5, 8}, "annotation changed the automatic limits");

        // A plot holding only annotations keeps the existing fallback view.
        plot only(plot::auto_limits);
        sg_group label(agg::trans_affine_scaling(20, 20));
        label.add(leaf(3, {0, 0, 1, 1}));
        only.add(std::make_unique<sg_annotation>(std::move(label), 7, 9));
        only.get_bounding_rect(box);
        same_rect(box, {0, 0, 0, 0}, "annotation-only plot invented data limits");
        order(render(only, true), {3}, "annotation-only plot drew nothing");
    }
    {   // Resize and limit changes: data geometry scales, annotations do not.
        plot p(0);
        p.set_limits({0, 0, 10, 10});
        p.add(std::move(group_of({1}, agg::trans_affine_translation(5, 5))));
        sg_group label(agg::trans_affine_scaling(24, 24));
        label.add(leaf(2, {0, 0, 1, 1}));
        p.add(std::make_unique<sg_annotation>(std::move(label), 5, 5));

        auto extent = [](const agg::rect_d& r) { return r.x2 - r.x1; };
        const auto before = render(p, true);
        p.set_limits({0, 0, 5, 5});
        const auto after = render(p, true);
        require(extent(after.paints[0].bounds) > 1.9 * extent(before.paints[0].bounds),
                "data-space group did not follow the new limits");
        require(std::abs(extent(after.paints[1].bounds) - 24) < 1e-8 &&
                std::abs(extent(before.paints[1].bounds) - 24) < 1e-8,
                "annotation geometry followed the limits");
        require(std::abs(after.paints[1].bounds.x1 - before.paints[1].bounds.x1) > 1,
                "annotation anchor ignored the new limits");
    }
}

void ownership() {
    counts c;
    {
        sg_group source(agg::trans_affine_translation(3, 4));
        source.add(counted_leaf(c, 1));
        auto child = std::make_unique<sg_group>(agg::trans_affine_scaling(2, 2));
        child->add(counted_leaf(c, 2));
        source.add(std::move(child));
        const auto expected = render(source, agg::trans_affine());

        sg_group copy(source);
        require(c.live() == 4 && c.copies == 2, "group copy is not deep");
        auto cloned = source.clone();
        require(c.live() == 6 && c.copies == 4, "group clone is not deep");

        sg_group moved(std::move(copy));
        require(c.live() == 6 && c.copies == 4, "group move cloned or leaked");

        // The copies outlive their source and keep drawing the same snapshot.
        {
            sg_group temporary(source);
            require(c.live() == 8, "group copy did not allocate");
        }
        require(c.live() == 6, "group copy destruction leaked");
        order(render(moved, agg::trans_affine()), {1, 2}, "moved group lost its children");
        same_rect(render(moved, agg::trans_affine()).paints[1].bounds,
                  expected.paints[1].bounds, "moved group lost a transform");
        same_rect(render(*cloned, agg::trans_affine()).paints[1].bounds,
                  expected.paints[1].bounds, "cloned group lost a transform");

        // Editing a builder after a snapshot does not change the snapshot.
        source.add(counted_leaf(c, 9));
        order(render(moved, agg::trans_affine()), {1, 2}, "snapshot follows its source");

        // A failed child clone propagates and leaves no partial copy behind.
        for (int i = 0; i < 2; ++i) {
            c.fail_after = i;
            const int copies = c.copies, live = c.live();
            bool thrown = false;
            try { sg_group failed(moved); } catch (const clone_failure&) { thrown = true; }
            require(thrown, "group clone exception was not propagated");
            require(c.live() == live && c.copies == copies + i,
                    "failed group copy leaked partial clones");
        }
        c.fail_after = -1;
        order(render(moved, agg::trans_affine()), {1, 2}, "failed copy damaged its source");
    }
    require(c.live() == 0, "group destruction leaked");

    {   // Plot insertion takes ownership, including through annotations.
        plot p(0);
        p.set_limits({0, 0, 10, 10});
        sg_group group;
        group.add(counted_leaf(c, 1));
        p.add(std::make_unique<sg_group>(group));
        p.add(std::make_unique<sg_annotation>(group, 1, 1));
        require(c.live() == 3, "plot insertion did not snapshot the group");
        p.commit_pending_draw();
        p.clear_current_layer();
        require(c.live() == 1, "plot clear did not release group ownership");
    }
    require(c.live() == 0, "plot destruction leaked group geometry");
}

void raster_and_svg() {
    // A group scaling the geometry of a stroked line, with a dashed sibling.
    auto make = [] {
        auto group = std::make_unique<sg_group>(agg::trans_affine(8, 0, 0, 1, 12, 20));
        auto line = std::make_unique<elem_path>();
        line->move_to(0, 0); line->line_to(2, 0);
        group->add(sg_element(line.release(), colors::black(), 8, colors::zero(), stroke));
        return group;
    };

    for (int scale : {1, 2}) { // Logical bounds must hold at HiDPI too.
        const int size = 64 * scale;
        std::vector<unsigned char> pixels(size * size * 3, 255);
        agg::rendering_buffer buffer(pixels.data(), size, size, size * 3);
        canvas_gen<renderer_gray_aa<pixel_simple>, pixel_simple> raster(buffer, scale, scale, colors::white());
        canvas_adapter<decltype(raster)> canvas(&raster);
        auto group = make();
        opt_rect<double> painted;
        group->draw(canvas, agg::trans_affine(), &painted);
        require(painted.is_defined(), "group produced no painted bounds");
        bool any = false;
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                if (pixels[(y * size + x) * 3] != 255) {
                    any = true;
                    require(x >= std::floor(painted.rect().x1 * scale) - 1 &&
                            x <= std::ceil(painted.rect().x2 * scale) + 1 &&
                            y >= std::floor(painted.rect().y1 * scale) - 1 &&
                            y <= std::ceil(painted.rect().y2 * scale) + 1,
                            "group raster paint escapes its logical painted bounds");
                }
            }
        }
        require(any, "group rendered no pixels");
    }

    std::unique_ptr<FILE, decltype(&std::fclose)> file(std::tmpfile(), &std::fclose);
    require(bool(file), "cannot create temporary SVG");
    canvas_svg svg(file.get(), 64);
    canvas_adapter<canvas_svg> canvas(&svg);
    svg.write_header(64, 64);
    auto group = make();
    auto dashes = std::make_unique<elem_dashed>();
    dashes->move_to(0, 0); dashes->line_to(2, 0);
    dashes->add_dash(3, 2);
    group->add(sg_element(dashes.release(), colors::black(), 1, colors::zero(), stroke));
    group->draw(canvas, agg::trans_affine());
    svg.write_end();
    std::rewind(file.get());
    std::string output;
    char chunk[1024];
    while (size_t read = std::fread(chunk, 1, sizeof(chunk), file.get())) output.append(chunk, read);
    require(output.find("stroke-width:8") != std::string::npos,
            "group lost the specialized SVG stroke dispatch");
    require(output.find("stroke-dasharray:3,2") != std::string::npos,
            "group lost the specialized SVG dash dispatch");
    require(output.find("12,44") != std::string::npos && output.find("28,44") != std::string::npos,
            "SVG group placement differs from the raster placement");
}

} // namespace

int main() {
    try {
        composition();
        bounds();
        screen_sized_styles();
        annotations();
        plot_integration();
        ownership();
        raster_and_svg();
    } catch (const std::exception& error) {
        std::fprintf(stderr, "group regression: %s\n", error.what());
        return 1;
    } catch (...) {
        std::fprintf(stderr, "group regression: unexpected exception\n");
        return 1;
    }
    return 0;
}

#include "elem/elem.h"
#include "elem/elem_c.h"
#include "canvas_path.h"

#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

using namespace elem;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

struct injected_failure {};
struct counts {
    int created = 0, destroyed = 0, copies = 0;
    int live() const { return created - destroyed; }
};

struct counted_path : elem_path {
    counts& count;
    explicit counted_path(counts& c) : count(c) {
        move_to(1, 2); line_to(3, 4);
        ++count.created;
    }
    ~counted_path() override { ++count.destroyed; }
    elem_object* copy() const override {
        auto result = new counted_path(count);
        ++count.copies;
        return result;
    }
};

struct tracked_object : Object {
    explicit tracked_object(counts& c) : Object(new counted_path(c)) {}
    elem_object* get() const { return object_impl_; }
};

Point apply(const Affine& m, Point p) {
    return Point{m.a * p.x + m.c * p.y + m.tx, m.b * p.x + m.d * p.y + m.ty};
}

elem_point apply(const elem_affine& m, elem_point p) {
    elem_point result = {m.a * p.x + m.c * p.y + m.tx, m.b * p.x + m.d * p.y + m.ty};
    return result;
}

void same_point(Point a, Point b, const char* message) {
    require(std::abs(a.x - b.x) < 1e-9 && std::abs(a.y - b.y) < 1e-9, message);
}

std::string temporary_name() {
    static int counter = 0;
    return "test-public-group-" + std::to_string(++counter) + ".svg";
}

std::string read_and_remove(const std::string& name) {
    std::string output;
    if (FILE* file = std::fopen(name.c_str(), "r")) {
        char chunk[1024];
        while (size_t read = std::fread(chunk, 1, sizeof(chunk), file)) output.append(chunk, read);
        std::fclose(file);
    }
    std::remove(name.c_str());
    require(!output.empty(), "temporary SVG is empty");
    return output;
}

// Rendering every plot to SVG keeps the checks independent of the plot layout.
std::string to_svg(Plot& plot) {
    const std::string name = temporary_name();
    require(plot.WriteSvg(name.c_str(), 320, 240), "cannot write a temporary SVG");
    return read_and_remove(name);
}

std::string to_svg(elem_plot* plot) {
    const std::string name = temporary_name();
    require(elem_plot_write_svg(plot, name.c_str(), 320, 240), "cannot write a temporary SVG");
    return read_and_remove(name);
}

// The first drawn path element, to compare one drawing inside larger outputs.
std::string first_path(const std::string& svg) {
    const size_t start = svg.find("<path");
    require(start != std::string::npos, "SVG contains no path element");
    const size_t end = svg.find("/>", start);
    require(end != std::string::npos, "SVG path element is unterminated");
    return svg.substr(start, end + 2 - start);
}

Plot flat_plot() {
    Plot plot{Plot::Options{0}};
    plot.SetLimits(0, 0, 10, 10);
    return plot;
}

void affine_algebra() {
    const Point p{3, 5};
    same_point(apply(Affine::Identity(), p), p, "identity is not the identity");
    same_point(apply(Affine::Translation(2, -4), p), {5, 1}, "translation coefficients are wrong");
    same_point(apply(Affine::Scale(2, 3), p), {6, 15}, "scale coefficients are wrong");
    same_point(apply(Affine::Scale(2), p), {6, 10}, "uniform scale is wrong");
    same_point(apply(Affine::Rotation(M_PI / 2), {1, 0}), {0, 1},
               "rotation turns the wrong way or is not in radians");
    same_point(apply(Affine{1, 0, 0.5, 1, 0, 0}, p), {5.5, 5},
               "six-coefficient construction does not follow x' = a*x + c*y + tx");

    // Then(next) applies this transform before next.
    const Affine first = Affine::Scale(2, 3), second = Affine::Translation(10, 20);
    same_point(apply(first.Then(second), p), apply(second, apply(first, p)),
               "Then composes in the wrong order");
    const Affine rotate = Affine::Rotation(0.4);
    require(std::abs(apply(rotate.Then(second), p).x - apply(second.Then(rotate), p).x) > 1e-6,
            "noncommuting composition collapsed");

    // The C helpers agree with the C++ value type.
    const elem_point q = {3, 5};
    const elem_affine c_first = elem_affine_scaling(2, 3);
    const elem_affine c_second = elem_affine_translation(10, 20);
    const elem_point composed = apply(elem_affine_then(c_first, c_second), q);
    same_point({composed.x, composed.y}, apply(first.Then(second), p),
               "the C and C++ affine helpers disagree");
    const elem_point turned = apply(elem_affine_rotation(0.4), q);
    same_point({turned.x, turned.y}, apply(rotate, p), "the C and C++ rotations disagree");
    const elem_point kept = apply(elem_affine_identity(), q);
    same_point({kept.x, kept.y}, p, "the C identity is not the identity");
}

void placement_matches_flat_geometry() {
    {   // A data-space group draws exactly what the same geometry drawn flat does.
        Plot expected = flat_plot();
        expected.AddStroke(Path{{10, 20}, {12, 20}, {12, 23}}, color::Blue, 1.5);

        Group symbol;
        symbol.AddStroke(Path{{0, 0}, {1, 0}, {1, 1}}, color::Blue, 1.5);
        Plot actual = flat_plot();
        actual.AddGroup(symbol, Affine::Scale(2, 3).Then(Affine::Translation(10, 20)));
        require(to_svg(actual) == to_svg(expected),
                "a placed group differs from the same geometry placed by hand");
    }
    {   // Subgroup placement composes with the outer placement, applied once.
        Plot expected = flat_plot();
        expected.AddStroke(Path{{11, 20}, {11, 22}}, color::Red, 1);

        Group inner;
        inner.AddStroke(Path{{0, 0}, {1, 0}}, color::Red, 1);
        Group outer;
        // Rotate the unit segment onto +y, then place the assembly.
        outer.AddGroup(inner, Affine::Rotation(M_PI / 2));
        Plot actual = flat_plot();
        actual.AddGroup(outer, Affine::Scale(1, 2).Then(Affine::Translation(11, 20)));
        require(to_svg(actual) == to_svg(expected), "nested group placement is wrong");
    }
    {   // Reusing one builder at two placements yields two independent snapshots.
        Plot expected = flat_plot();
        expected.AddStroke(Path{{1, 1}, {2, 1}}, color::Black, 1);
        expected.AddStroke(Path{{5, 5}, {6, 5}}, color::Black, 1);

        Group unit;
        unit.AddStroke(Path{{0, 0}, {1, 0}}, color::Black, 1);
        Plot actual = flat_plot();
        actual.AddGroup(unit, Affine::Translation(1, 1));
        actual.AddGroup(unit, Affine::Translation(5, 5));
        require(to_svg(actual) == to_svg(expected), "a reused group shares a placement");

        // Editing the builder afterwards does not change the inserted snapshots.
        unit.AddStroke(Path{{0, 0}, {0, 9}}, color::Green, 1);
        require(to_svg(actual) == to_svg(expected), "insertion did not snapshot the group");
    }
    {   // The C API builds the same plot as the C++ one.
        Group group;
        group.AddStroke(Path{{0, 0}, {1, 0}}, color::Blue, 1.5);
        Plot expected = flat_plot();
        expected.AddGroup(group, Affine::Scale(4, 4).Then(Affine::Translation(2, 3)));

        elem_plot* plot = elem_plot_new(0);
        elem_plot_set_limits(plot, 0, 0, 10, 10);
        elem_group* c_group = elem_group_new();
        elem_path* path = elem_path_new();
        elem_path_move_to(path, 0, 0);
        elem_path_line_to(path, 1, 0);
        elem_group_add(c_group, path_as_object(path), elem_color_blue, 1.5, elem_color_none, elem_stroke);
        elem_plot_add_group(plot, c_group,
                            elem_affine_then(elem_affine_scaling(4, 4), elem_affine_translation(2, 3)));
        require(to_svg(plot) == to_svg(expected), "the C and C++ group APIs disagree");
        elem_plot_free(plot);
    }
}

void annotation_placement() {
    Group label;
    label.AddStroke(Path{{0, 0}, {1, 0}}, color::Black, 1);

    Plot plot = flat_plot();
    plot.AddAnnotation(label, Point{5, 5}, Affine::Scale(24, 24));
    const std::string placed = to_svg(plot);

    // Changing the limits moves the anchor without rescaling the local layout.
    Plot rescaled = flat_plot();
    rescaled.SetLimits(0, 0, 5, 5);
    rescaled.AddAnnotation(label, Point{5, 5}, Affine::Scale(24, 24));
    require(to_svg(rescaled) != placed, "the annotation anchor ignored the new limits");

    // The same local layout drawn as data geometry does follow the limits.
    Plot as_data = flat_plot();
    as_data.AddGroup(label, Affine::Scale(24, 24));
    require(to_svg(as_data) != placed, "annotation and data placement are indistinguishable");

    // Annotations do not change automatic limits: the data drawing is untouched.
    Plot automatic;
    automatic.AddStroke(Path{{0, 0}, {1, 1}}, color::Red, 1);
    const std::string before = to_svg(automatic);
    automatic.AddAnnotation(label, Point{1000, 1000}, Affine::Scale(24, 24));
    const std::string after = to_svg(automatic);
    require(after.find(first_path(before)) != std::string::npos,
            "adding an annotation changed the automatic limits");
    require(after.size() > before.size(), "the annotation was not drawn");

    // A data-space group at the same place would move the view instead.
    Plot enlarged;
    enlarged.AddStroke(Path{{0, 0}, {1, 1}}, color::Red, 1);
    enlarged.AddGroup(label, Affine::Scale(24, 24).Then(Affine::Translation(1000, 1000)));
    require(to_svg(enlarged).find(first_path(before)) == std::string::npos,
            "a data-space group did not change the automatic limits");
}

void screen_bounds() {
    Group line;
    line.AddStroke(Path{{0, 0}, {10, 0}}, color::Black, 4);

    Bounds bounds;
    require(line.GetScreenBounds(bounds), "line group has no screen bounds");
    require(bounds.Width() > 10 && bounds.Height() > 3,
            "screen bounds omitted the stroke extent");

    Bounds translated;
    require(line.GetScreenBounds(translated, Affine::Translation(7, 9)),
            "translated line group has no screen bounds");
    require(std::abs(translated.Width() - bounds.Width()) < 1e-9 &&
            std::abs(translated.Height() - bounds.Height()) < 1e-9 &&
            std::abs(translated.x1 - bounds.x1 - 7) < 1e-9 &&
            std::abs(translated.y1 - bounds.y1 - 9) < 1e-9,
            "nominal translation changed the wrong screen bounds");

    Group text_group;
    Text label{"measure me", 14};
    label.SetPosition(10, 20);
    text_group.Add(label, color::Black, 0, color::Black, property::Fill);

    Bounds text_bounds;
    require(text_group.GetScreenBounds(text_bounds), "text group has no screen bounds");
    require(text_bounds.Width() > 0 && text_bounds.Height() > 0,
            "text screen bounds are empty");

    Bounds text_scaled;
    require(text_group.GetScreenBounds(text_scaled, Affine::Scale(3, 4)),
            "scaled text group has no screen bounds");
    require(std::abs(text_scaled.Width() - text_bounds.Width()) < 1e-9 &&
            std::abs(text_scaled.Height() - text_bounds.Height()) < 1e-9,
            "nominal transform scaled text glyphs");

    Group empty;
    require(!empty.GetScreenBounds(bounds), "empty group has screen bounds");

    elem_group* c_group = elem_group_new();
    elem_path* c_line = elem_path_new();
    elem_path_move_to(c_line, 0, 0);
    elem_path_line_to(c_line, 10, 0);
    elem_group_add(c_group, path_as_object(c_line), elem_color_black, 4,
                   elem_color_none, elem_stroke);
    elem_bounds c_bounds;
    require(elem_group_get_screen_bounds(c_group, elem_affine_identity(), &c_bounds),
            "C group has no screen bounds");
    require(std::abs(double(c_bounds.x1) - bounds.x1) < 1e-9 &&
            std::abs(double(c_bounds.y1) - bounds.y1) < 1e-9 &&
            std::abs(double(c_bounds.x2) - bounds.x2) < 1e-9 &&
            std::abs(double(c_bounds.y2) - bounds.y2) < 1e-9,
            "C and C++ screen bounds disagree");
    elem_group_free(c_group);
}

Legend make_legend() {
    Legend legend;
    Group entry;
    entry.AddStroke(Path{{0, 0}, {16, 0}}, color::Blue, 1.5);
    Text label{"legend entry", 12};
    label.SetPosition(24, 0);
    entry.Add(label, color::None, 0, color::Black, property::Fill);
    legend.Add(entry);
    return legend;
}

void legend_api() {
    Plot plot = flat_plot();
    Legend source = make_legend();
    plot.SetLegend(source, LegendLocation::InsideTopLeft);
    const std::string before = to_svg(plot);

    // Both legend and plot insertion are snapshots.
    Group extra;
    extra.AddStroke(Path{{0, 0}, {8, 0}}, color::Red, 1);
    source.Add(std::move(extra));
    require(to_svg(plot) == before, "editing a legend source changed the plot");
    Plot copied = plot;
    require(to_svg(copied) == before, "plot copy lost its grouped legend");
    plot.ClearLegend(LegendLocation::InsideTopLeft);
    require(to_svg(plot) != before, "clearing a legend did not redraw the plot");

    elem_plot* c_plot = elem_plot_new(0);
    elem_plot_set_limits(c_plot, 0, 0, 10, 10);
    elem_group* c_entry = elem_group_new();
    elem_path* c_path = elem_path_new();
    elem_path_move_to(c_path, 0, 0);
    elem_path_line_to(c_path, 16, 0);
    elem_group_add(c_entry, path_as_object(c_path), elem_color_blue, 1.5,
                   elem_color_none, elem_stroke);
    elem_object* c_text = elem_text_new("legend entry", 12, 0, 0);
    elem_text_set_position(c_text, 24, 0);
    elem_group_add(c_entry, c_text, elem_color_none, 0, elem_color_black,
                   elem_fill);
    elem_legend* c_legend = elem_legend_new();
    elem_legend_add_group(c_legend, c_entry);
    elem_plot_set_legend(c_plot, c_legend, elem_plot_inside_top_left);
    const std::string c_output = to_svg(c_plot);
    require(c_output.find("legend entry") != std::string::npos,
            "C grouped legend did not render its text");
    elem_plot_clear_legend(c_plot, elem_plot_inside_top_left);
    require(to_svg(c_plot).find("legend entry") == std::string::npos,
            "C legend clearing did not remove the entry");
    elem_plot_free(c_plot);
}

void ownership() {
    counts geometry;
    {
        Group group;
        group.Add(tracked_object(geometry), color::Red, 1, color::None);
        require(geometry.live() == 1 && geometry.copies == 0, "Group::Add cloned the geometry");

        // Passing an lvalue copies; the copy is independent of its source.
        Group copy(group);
        require(geometry.live() == 2 && geometry.copies == 1, "Group copy is not deep");
        Group moved(std::move(group));
        require(geometry.live() == 2 && geometry.copies == 1, "Group move cloned or leaked");

        Group assigned;
        assigned.Add(tracked_object(geometry), color::Blue, 1, color::None);
        assigned = copy;
        require(geometry.live() == 3 && geometry.copies == 2,
                "Group assignment leaked the old children");
        assigned = std::move(copy);
        require(geometry.live() == 2 && geometry.copies == 2, "Group move assignment cloned");

        // A moved-from Group is inert rather than a crash.
        Group empty_source(std::move(assigned));
        Group from_moved(assigned);
        assigned.Add(tracked_object(geometry), color::Red, 1, color::None);
        assigned.AddGroup(from_moved);
        Plot plot = flat_plot();
        plot.AddGroup(assigned);
        plot.AddAnnotation(assigned, Point{0, 0});
        require(geometry.live() == 2, "a moved-from Group retained children");

        // Insertion takes ownership without cloning a moved value.
        Plot owner = flat_plot();
        owner.AddGroup(std::move(empty_source), Affine::Translation(1, 1));
        require(geometry.live() == 2 && geometry.copies == 2, "AddGroup cloned a moved Group");
        owner.ClearLayer();
        require(geometry.live() == 1, "ClearLayer did not release the inserted group");

        Group subgroup;
        subgroup.Add(tracked_object(geometry), color::Red, 1, color::None);
        Group parent;
        parent.AddGroup(subgroup, Affine::Translation(2, 2));
        require(geometry.live() == 3 && geometry.copies == 3, "AddGroup did not copy an lvalue");
        parent.AddGroup(std::move(subgroup));
        require(geometry.live() == 3 && geometry.copies == 3, "AddGroup cloned a moved subgroup");
    }
    require(geometry.live() == 0, "public group lifetime leaked");

    {   // The C API consumes its arguments, and releases them on failure too.
        counts c;
        elem_group* group = elem_group_new();
        elem_group_add(group, new counted_path(c), elem_color_red, 1, elem_color_none, elem_stroke);
        require(c.live() == 1, "elem_group_add did not take ownership");

        elem_group* copy = elem_group_copy(group);
        require(c.live() == 2 && c.copies == 1, "elem_group_copy is not deep");
        elem_group_add_group(group, copy, elem_affine_identity());
        require(c.live() == 2 && c.copies == 1, "elem_group_add_group cloned the child");

        elem_plot* plot = elem_plot_new(0);
        elem_plot_set_limits(plot, 0, 0, 10, 10);
        elem_group* rejected = elem_group_copy(group);
        const elem_affine bad = {1, 0, 0, 1, HUGE_VAL, 0};
        bool thrown = false;
        try { elem_plot_add_group(plot, rejected, bad); }
        catch (const std::invalid_argument&) { thrown = true; }
        require(thrown, "a non-finite placement was accepted");
        require(c.live() == 2, "a rejected placement leaked the group");

        elem_plot_add_group(plot, group, elem_affine_identity());
        require(c.live() == 2, "elem_plot_add_group cloned the group");
        elem_plot_clear_layer(plot);
        require(c.live() == 0, "elem_plot_clear_layer did not release the group");
        elem_plot_free(plot);
    }
}

} // namespace

// Link with libelemwin_dep for backend symbols, but never initialize the GUI.
int main() {
    try {
        // Writing SVG renders the axis labels, which need the text backend.
        InitializeFonts();
        affine_algebra();
        placement_matches_flat_geometry();
        annotation_placement();
        screen_bounds();
        legend_api();
        ownership();
    } catch (const std::exception& error) {
        std::fprintf(stderr, "public group regression: %s\n", error.what());
        return 1;
    } catch (...) {
        std::fprintf(stderr, "public group regression: unexpected exception\n");
        return 1;
    }
    return 0;
}

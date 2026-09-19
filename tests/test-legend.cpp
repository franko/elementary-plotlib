#include "plot.h"
#include "sg_group.h"
#include "canvas_path.h"

#include <cmath>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <vector>

namespace {

using graphics::legend;
using graphics::legend_direction;
using graphics::plot;
using namespace graphics::property;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

struct recording_canvas : virtual_canvas {
    struct paint {
        int id;
        agg::rect_d bounds;
    };

    std::vector<paint> paints;

    void record(elem_object& object, agg::rgba8 color) {
        agg::rect_d bounds;
        require(agg::bounding_rect_single(object, 0, &bounds.x1, &bounds.y1,
                                          &bounds.x2, &bounds.y2),
                "legend paint has no geometry");
        paints.push_back({color.r, bounds});
    }

    void draw(elem_object& object, agg::rgba8 color) override {
        record(object, color);
    }
    void draw_outline_noaa(elem_object& object, agg::rgba8 color) override {
        record(object, color);
    }
    void draw_noaa(elem_object& object, agg::rgba8 color) override {
        record(object, color);
    }
    void clip_logical_area(const agg::rect_base<int>&) override { }
    void reset_clipping() override { }
};

sg_group line_entry(int id, double width = 20.0) {
    auto path = std::make_unique<elem_path>();
    path->move_to(0, 0);
    path->line_to(width, 0);
    sg_group entry;
    entry.add(sg_element(path.release(), agg::rgba8(id, 0, 0, 255), 2,
                         agg::rgba8(0, 0, 0, 0), stroke));
    return entry;
}

sg_group data_line() {
    auto path = std::make_unique<elem_path>();
    path->move_to(0, 0);
    path->line_to(1, 1);
    sg_group entry;
    entry.add(sg_element(path.release(), agg::rgba8(99, 0, 0, 255), 1,
                         agg::rgba8(0, 0, 0, 0), stroke));
    return entry;
}

recording_canvas render(plot& value) {
    recording_canvas canvas;
    plot::drawing_context context(value);
    value.draw(context, canvas, agg::trans_affine(640, 0, 0, 480, 0, 0), nullptr);
    return canvas;
}

void install(plot& value, int location) {
    auto grouped = std::make_unique<legend>();
    grouped->add(line_entry(1));
    grouped->add(line_entry(2, 10.0));
    value.set_legend(std::move(grouped), location);
}

void outside_direction() {
    plot right(0);
    right.set_limits({0, 0, 1, 1});
    right.add(std::make_unique<sg_group>(data_line()));
    install(right, plot_legend_right);
    right.commit_pending_draw();
    const recording_canvas vertical = render(right);
    require(vertical.paints.size() == 3, "right legend paint count is wrong");
    require(vertical.paints[0].id == 1 && vertical.paints[1].id == 2 &&
            vertical.paints[2].id == 99, "right legend order is wrong");
    require(vertical.paints[0].bounds.y1 > vertical.paints[1].bounds.y1,
            "right legend did not form a top-to-bottom column");
    require(std::abs(vertical.paints[0].bounds.x1 - vertical.paints[1].bounds.x1) < 1e-8,
            "right legend entries are not left aligned");

    plot bottom(0);
    bottom.set_limits({0, 0, 1, 1});
    bottom.add(std::make_unique<sg_group>(data_line()));
    install(bottom, plot_legend_bottom);
    bottom.commit_pending_draw();
    const recording_canvas horizontal = render(bottom);
    require(horizontal.paints.size() == 3, "bottom legend paint count is wrong");
    require(horizontal.paints[0].id == 1 && horizontal.paints[1].id == 2 &&
            horizontal.paints[2].id == 99, "bottom legend order is wrong");
    require(horizontal.paints[0].bounds.x1 < horizontal.paints[1].bounds.x1,
            "bottom legend did not form a horizontal row");
}

void inside_order_and_limits() {
    plot value(plot::auto_limits);
    value.add(std::make_unique<sg_group>(data_line()));
    agg::rect_d before;
    value.get_bounding_rect(before);

    install(value, plot_legend_inside_top_right);
    agg::rect_d after;
    value.get_bounding_rect(after);
    require(before.x1 == after.x1 && before.y1 == after.y1 &&
            before.x2 == after.x2 && before.y2 == after.y2,
            "legend content changed automatic limits");

    value.commit_pending_draw();
    const recording_canvas rendered = render(value);
    require(rendered.paints.size() == 3, "inside legend paint count is wrong");
    require(rendered.paints[0].id == 99 && rendered.paints[1].id == 1 &&
            rendered.paints[2].id == 2,
            "inside legend was not painted after data");
    require(std::abs(rendered.paints[1].bounds.x1 - rendered.paints[2].bounds.x1) < 1e-8,
            "inside legend entries are not left aligned");
    require(rendered.paints[1].bounds.y1 > rendered.paints[2].bounds.y1,
            "inside top-right legend is vertically inverted");
}

void inside_edge_directions() {
    auto render_location = [](int location) {
        plot value(0);
        value.set_limits({0, 0, 1, 1});
        value.add(std::make_unique<sg_group>(data_line()));
        install(value, location);
        value.commit_pending_draw();
        return render(value);
    };

    const recording_canvas top = render_location(plot_legend_inside_top);
    const recording_canvas bottom = render_location(plot_legend_inside_bottom);
    require(top.paints[1].bounds.x1 < top.paints[2].bounds.x1 &&
            bottom.paints[1].bounds.x1 < bottom.paints[2].bounds.x1,
            "inside top/bottom legends did not use horizontal layout");
    require(std::abs(top.paints[1].bounds.y1 - top.paints[2].bounds.y1) < 1e-8 &&
            std::abs(bottom.paints[1].bounds.y1 - bottom.paints[2].bounds.y1) < 1e-8,
            "inside top/bottom legend entries are not in one row");

    const recording_canvas left = render_location(plot_legend_inside_left);
    const recording_canvas right = render_location(plot_legend_inside_right);
    require(left.paints[1].bounds.y1 > left.paints[2].bounds.y1 &&
            right.paints[1].bounds.y1 > right.paints[2].bounds.y1,
            "inside left/right legends did not use vertical layout");
    require(std::abs(left.paints[1].bounds.x1 - left.paints[2].bounds.x1) < 1e-8 &&
            std::abs(right.paints[1].bounds.x1 - right.paints[2].bounds.x1) < 1e-8,
            "inside left/right legend entries are not left aligned");
}

void empty_entries() {
    legend value;
    value.add(sg_group{});
    require(value.empty(), "empty legend entry reserved space");
    const graphics::legend_size measured = value.measure(legend_direction::vertical);
    require(measured.width == 0 && measured.height == 0,
            "empty legend has measured size");
}

} // namespace

int main() {
    try {
        outside_direction();
        inside_order_and_limits();
        inside_edge_directions();
        empty_entries();
    } catch (const std::exception& error) {
        std::fprintf(stderr, "legend regression: %s\n", error.what());
        return 1;
    }
    return 0;
}

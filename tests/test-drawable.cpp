#include "plot.h"
#include "canvas_path.h"
#include "canvas_markers.h"
#include "canvas_svg.h"

#include <cmath>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void same_rect(const opt_rect<double>& bounds, const agg::rect_d& r) {
    require(bounds.is_defined(), "missing bounds");
    const auto& b = bounds.rect();
    require(std::abs(b.x1 - r.x1) < 1e-8 && std::abs(b.y1 - r.y1) < 1e-8 &&
            std::abs(b.x2 - r.x2) < 1e-8 && std::abs(b.y2 - r.y2) < 1e-8,
            "incorrect bounds");
}

struct recording_canvas : virtual_canvas {
    std::vector<agg::rect_d> bounds;
    void draw(elem_object& object, agg::rgba8) override {
        agg::rect_d r;
        if (agg::bounding_rect_single(object, 0, &r.x1, &r.y1, &r.x2, &r.y2)) {
            bounds.push_back(r);
        }
    }
    void draw_noaa(elem_object& o, agg::rgba8 c) override { draw(o, c); }
    void draw_outline_noaa(elem_object& o, agg::rgba8 c) override { draw(o, c); }
    void clip_logical_area(const agg::rect_i&) override { }
    void reset_clipping() override { }
};

std::unique_ptr<elem_path> line() {
    auto path = std::make_unique<elem_path>();
    path->move_to(0, 0);
    path->line_to(2, 0);
    return path;
}

void placement_and_bounds() {
    recording_canvas canvas;
    const agg::trans_affine local(0, 1, -1, 0, 5, 7);
    const agg::trans_affine parent(2, 0, 0, 3, 11, 13);
    sg_element leaf(line().release(), colors::black(), 8, colors::zero(),
                    graphics::property::stroke, local);
    same_rect(leaf.data_bounds(), {5, 7, 5, 9});
    opt_rect<double> painted;
    leaf.draw(canvas, parent, &painted);
    require(painted.is_defined(), "missing stroke bounds");
    // The line maps to (21,34)..(21,40); its 8-pixel stroke does not scale.
    const auto& b = painted.rect();
    // Round caps are polygonal approximations with subpixel tolerance.
    require(std::abs(b.x1 - 17) < .125 && std::abs(b.x2 - 25) < .125 &&
            std::abs(b.y1 - 30) < .125 && std::abs(b.y2 - 44) < .125,
            "stroke width scaled or painted bounds omitted caps");
    same_rect(leaf.data_bounds(), {5, 7, 5, 9});
    leaf.draw(canvas, parent, &painted);
    same_rect(painted, canvas.bounds.front());
    auto clone = leaf.clone();
    clone->draw(canvas, parent, &painted);
    same_rect(painted, canvas.bounds.front());

    auto box = std::make_unique<elem_path>();
    box->move_to(-2, -1); box->line_to(3, -1);
    box->line_to(3, 4); box->line_to(-2, 4); box->close_polygon();
    sg_element reflected(box.release(), colors::zero(), 0, colors::black(),
                         graphics::property::fill, agg::trans_affine(-2, .5, 1, 3, 10, -7));
    same_rect(reflected.data_bounds(), {3, -11, 18, 6.5});

    sg_element empty(new elem_path, colors::black(), 10, colors::black(),
                     graphics::property::fill | graphics::property::stroke);
    require(!empty.data_bounds().is_defined(), "empty geometry contributes data bounds");
    empty.draw(canvas, parent, &painted);
    require(!painted.is_defined(), "empty geometry contributes painted bounds");
    sg_element invisible(line().release(), colors::zero(), 10, colors::zero(),
                         graphics::property::fill | graphics::property::stroke);
    invisible.draw(canvas, parent, &painted);
    require(!painted.is_defined(), "invisible geometry contributes painted bounds");
    same_rect(invisible.data_bounds(), {0, 0, 2, 0});
}

void fixed_size_styles() {
    recording_canvas canvas;
    auto markers = std::make_unique<elem_markers>(10, new_marker_symbol(2));
    markers->move_to(2, 3);
    sg_element leaf(markers.release(), colors::zero(), 0, colors::black(), graphics::property::fill);
    same_rect(leaf.data_bounds(), {2, 3, 2, 3});
    opt_rect<double> before, after;
    leaf.draw(canvas, agg::trans_affine(), &before);
    leaf.draw(canvas, agg::trans_affine(4, 0, 0, 7, 0, 0), &after);
    require(std::abs((before.rect().x2 - before.rect().x1) -
                     (after.rect().x2 - after.rect().x1)) < 1e-8,
            "marker size scaled with placement");
    same_rect(leaf.data_bounds(), {2, 3, 2, 3});
}

void raster_and_svg() {
    sg_element leaf(line().release(), colors::black(), 8, colors::zero(),
                    graphics::property::stroke, agg::trans_affine(8, 0, 0, 1, 12, 20));
    for (int scale : {1, 2}) {
        const int size = 64 * scale;
        std::vector<unsigned char> pixels(size * size * 3, 255);
        agg::rendering_buffer buffer(pixels.data(), size, size, size * 3);
        canvas_gen<renderer_gray_aa<pixel_simple>, pixel_simple> raster(buffer, scale, scale, colors::white());
        canvas_adapter<decltype(raster)> canvas(&raster);
        opt_rect<double> bounds;
        leaf.draw(canvas, agg::trans_affine(), &bounds);
        bool painted = false;
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                if (pixels[(y * size + x) * 3] != 255) {
                    painted = true;
                    require(x >= std::floor(bounds.rect().x1 * scale) - 1 &&
                            x <= std::ceil(bounds.rect().x2 * scale) + 1 &&
                            y >= std::floor(bounds.rect().y1 * scale) - 1 &&
                            y <= std::ceil(bounds.rect().y2 * scale) + 1,
                            "raster paint escapes logical painted bounds");
                }
            }
        }
        require(painted, "raster rendered no pixels");
    }

    std::unique_ptr<FILE, decltype(&std::fclose)> file(std::tmpfile(), &std::fclose);
    require(bool(file), "cannot create temporary SVG");
    canvas_svg svg(file.get(), 64);
    canvas_adapter<canvas_svg> canvas(&svg);
    svg.write_header(64, 64);
    leaf.draw(canvas, agg::trans_affine());
    svg.write_end();
    std::rewind(file.get());
    std::string output;
    char buffer[1024];
    while (size_t size = std::fread(buffer, 1, sizeof(buffer), file.get())) output.append(buffer, size);
    require(output.find("stroke-width:8") != std::string::npos,
            "SVG lost specialized stroke dispatch");
    require(output.find("12,44") != std::string::npos && output.find("28,44") != std::string::npos,
            "SVG placement differs from raster placement");
}

} // namespace

int main() {
    try {
        placement_and_bounds();
        fixed_size_styles();
        raster_and_svg();
    } catch (const std::exception& error) {
        std::fprintf(stderr, "drawable regression: %s\n", error.what());
        return 1;
    }
    return 0;
}

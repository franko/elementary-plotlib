#include "canvas_curve.h"
#include "canvas_dashed.h"
#include "canvas_markers.h"
#include "markers.h"
#include "trans.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

// Fail after elem_object_gen has allocated a partially constructed clone.
struct throwing_vertices {
    static int live;
    throwing_vertices() { ++live; }
    ~throwing_vertices() { --live; }
    void rewind(unsigned) {}
    unsigned vertex(double*, double*) { return agg::path_cmd_stop; }
};

int throwing_vertices::live = 0;

template <>
void vertex_source_copy(throwing_vertices&, const throwing_vertices&) {
    throw std::runtime_error("intentional vertex copy failure");
}

namespace {

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

template <class Exception, class Function>
void expect_exception(Function function, const char* message) {
    try {
        function();
    } catch (const Exception&) {
        return;
    }
    throw std::runtime_error(message);
}

struct vertex {
    unsigned command;
    double x, y;
};

std::vector<vertex> vertices(elem_object& object) {
    std::vector<vertex> result;
    object.rewind(0);
    for (unsigned i = 0; i < 100000; ++i) {
        double x = 0, y = 0;
        unsigned command = object.vertex(&x, &y);
        if (agg::is_stop(command)) return result;
        result.push_back({command, x, y});
    }
    throw std::runtime_error("vertex stream did not terminate");
}

void same_vertices(const std::vector<vertex>& expected, elem_object& object, const char* message) {
    const auto actual = vertices(object);
    require(actual.size() == expected.size(), message);
    for (std::size_t i = 0; i < expected.size(); ++i) {
        require(actual[i].command == expected[i].command, message);
        if (agg::is_vertex(expected[i].command)) {
            require(std::abs(actual[i].x - expected[i].x) < 1e-12 &&
                    std::abs(actual[i].y - expected[i].y) < 1e-12, message);
        }
    }
}

void check_clone(std::unique_ptr<elem_object> original, const char* message) {
    const auto expected = vertices(*original);
    const auto svg = original->write_svg(7, agg::rgba8(20, 40, 60), 100);
    std::unique_ptr<elem_object> clone(original->copy());
    require(clone && clone.get() != original.get(), message);
    original->apply_transform(agg::trans_affine_translation(100, -50), 0.75);
    original.reset();
    same_vertices(expected, *clone, message);
    require(std::strcmp(svg.cstr(), clone->write_svg(7, agg::rgba8(20, 40, 60), 100).cstr()) == 0,
            message);
    std::unique_ptr<elem_object> second(clone->copy());
    require(second && second.get() != clone.get(), message);
    clone.reset();
    same_vertices(expected, *second, message);
}

void check_copy_failure(elem_object& object) {
    expect_exception<std::runtime_error>([&] {
        std::unique_ptr<elem_object> clone(object.copy());
    }, "source clone exception was not propagated");
    require(throwing_vertices::live == 1, "failed clone leaked or destroyed its source");
}

} // namespace

int main() {
    try {
        const agg::trans_affine transform(1.5, 0.25, -0.4, 2.0, 3.0, -5.0);

        auto path = std::make_unique<elem_path>();
        path->move_to(0, 0);
        path->line_to(2, 3);
        path->line_to(-1, 4);
        path->close_polygon();
        path->apply_transform(transform, 7.25);
        check_clone(std::move(path), "transformed path clone differs");

        auto curve = std::make_unique<elem_curve>();
        curve->move_to(0, 0);
        curve->curve3(1, 7, 3, 2);
        curve->curve4(4, -5, 8, 9, 10, 1);
        curve->arc_to(2, 3, 0.4, false, true, 12, 4);
        curve->apply_transform(transform, 7.25);
        check_clone(std::move(curve), "transformed curve clone differs");

        auto dashed = std::make_unique<elem_dashed>();
        dashed->move_to(-1, 2);
        dashed->line_to(12, 5);
        dashed->line_to(15, -1);
        dashed->add_dash(0.123456789123, 0.234567891234);
        dashed->add_dash(1.345678912345, 0.456789123456);
        dashed->apply_transform(transform, 7.25);
        check_clone(std::move(dashed), "dash clone lost transform or double precision");

        const char* names[] = {"circle", "square", "triangle", "diamond", "plus", "cross"};
        for (int i = 1; i <= 6; ++i) {
            std::unique_ptr<elem_object> symbol(new_marker_symbol(i));
            require(bool(symbol), "marker factory returned an empty symbol");
            symbol->apply_transform(transform, 7.25);
            check_clone(std::move(symbol), names[i - 1]);

            std::unique_ptr<elem_object> named_symbol(new_marker_symbol(names[i - 1]));
            auto markers = std::make_unique<elem_markers>(4.25, named_symbol.get());
            named_symbol.release();
            markers->move_to(-1, 2);
            markers->line_to(3, 5);
            markers->apply_transform(transform, 7.25);
            check_clone(std::move(markers), names[i - 1]);
        }

        auto source = std::make_unique<elem_path>();
        source->move_to(0, 0);
        source->line_to(4, 0);
        source->line_to(1, 3);
        auto stroke = std::make_unique<graphics::transform::stroke>(source.get());
        source.release();
        stroke->apply_transform(transform, 7.25);
        stroke->width(2.75);
        // Keep the SVG width and directly configured converter width distinct.
        stroke->self().width(3.125);
        stroke->self().line_cap(agg::round_cap);
        stroke->self().line_join(agg::round_join);
        stroke->self().inner_join(agg::inner_round);
        stroke->self().miter_limit_theta(0.77);
        stroke->self().inner_miter_limit(2.5);
        stroke->self().approximation_scale(6.25);
        stroke->self().shorten(0.17);
        std::unique_ptr<elem_object> stroke_clone(stroke->copy());
        auto* copied_stroke = dynamic_cast<graphics::transform::stroke*>(stroke_clone.get());
        require(copied_stroke != nullptr, "stroke clone has the wrong type");
        const auto& a = stroke->self();
        const auto& b = copied_stroke->self();
        require(a.width() == b.width() && a.line_cap() == b.line_cap() &&
                a.line_join() == b.line_join() && a.inner_join() == b.inner_join() &&
                a.miter_limit() == b.miter_limit() && a.inner_miter_limit() == b.inner_miter_limit() &&
                a.approximation_scale() == b.approximation_scale() && a.shorten() == b.shorten(),
                "stroke clone lost converter settings");
        stroke_clone.reset();
        check_clone(std::move(stroke), "stroke clone differs");

        elem_path borrowed_path;
        sg_adapter<graphics::transform::conv_stroke, approx_scale> adapter(&borrowed_path);
        expect_exception<std::logic_error>([&] {
            std::unique_ptr<elem_object> clone(adapter.copy());
        }, "generic adapter clone did not throw logic_error");
        graphics::transform::scaling_a borrowed_scaling(&borrowed_path);
        expect_exception<std::logic_error>([&] {
            std::unique_ptr<elem_object> clone(borrowed_scaling.copy());
        }, "borrowed scaling clone did not throw logic_error");
        expect_exception<std::invalid_argument>([] {
            elem_markers markers(2, nullptr);
        }, "empty marker symbol was not rejected");
        expect_exception<std::invalid_argument>([&] {
            graphics::transform::marker_a markers(&borrowed_path, 2, nullptr);
        }, "empty adapter marker symbol was not rejected");

        {
            auto failing = std::make_unique<elem_object_gen<throwing_vertices>>();
            check_copy_failure(*failing);
            auto failing_stroke = std::make_unique<graphics::transform::stroke>(failing.get());
            failing.release();
            check_copy_failure(*failing_stroke);
            auto scaling = std::make_unique<graphics::transform::scaling>(failing_stroke.get());
            failing_stroke.release();
            check_copy_failure(*scaling);
            auto markers = std::make_unique<elem_markers>(2, scaling.get());
            scaling.release();
            check_copy_failure(*markers);
        }
        require(throwing_vertices::live == 0, "clone failure pipeline leaked its source");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "geometry copy regression: %s\n", error.what());
        return 1;
    }
    return 0;
}

#include "plot.h"
#include "canvas_path.h"

#include <cmath>
#include <cstdio>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

using graphics::plot;
using namespace graphics::property;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void same_rect(const agg::rect_d& a, const agg::rect_d& b, const char* message) {
    require(std::abs(a.x1 - b.x1) < 1e-9 && std::abs(a.y1 - b.y1) < 1e-9 &&
            std::abs(a.x2 - b.x2) < 1e-9 && std::abs(a.y2 - b.y2) < 1e-9, message);
}

struct clone_failure {};
struct counts {
    int created = 0, destroyed = 0, copies = 0, fail_after = -1;
    int live() const { return created - destroyed; }
};

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

sg_element element(counts& c, int id, const agg::rect_d& r = {1, 2, 3, 4},
                   unsigned flags = fill, const agg::trans_affine& local = {}) {
    return sg_element(new counted_box(c, r), ink(id), 2, ink(id), flags, local);
}

struct recording_canvas : virtual_canvas {
    struct paint { int id, mode; agg::rect_d bounds; };
    std::vector<paint> paints;
    opt_rect<double> painted, damage;
    plot_render_info info;

    void record(elem_object& object, agg::rgba8 color, int mode) {
        agg::rect_d r;
        require(agg::bounding_rect_single(object, 0, &r.x1, &r.y1, &r.x2, &r.y2),
                "painted geometry is empty");
        if (mode == 2) { r.x1 -= 1; r.y1 -= 1; r.x2 += 1; r.y2 += 1; }
        require(color.g == 23 && color.b == 45 && color.a == 255, "paint color changed");
        paints.push_back({color.r, mode, r});
        painted.add<rect_union>(r);
    }
    void draw(elem_object& o, agg::rgba8 c) override { record(o, c, 0); }
    void draw_noaa(elem_object& o, agg::rgba8 c) override { record(o, c, 1); }
    void draw_outline_noaa(elem_object& o, agg::rgba8 c) override { record(o, c, 2); }
    void clip_logical_area(const agg::rect_i&) override {}
    void reset_clipping() override {}
};

recording_canvas render(plot& p, bool queue = false) {
    recording_canvas canvas;
    plot::drawing_context context(p);
    const agg::trans_affine slot(640, 0, 0, 480, 13, 17);
    p.draw(context, canvas, slot, &canvas.info);
    if (queue) {
        // Obtain the real active area without including committed paints in the result.
        canvas.paints.clear(); canvas.painted.clear();
        p.draw_queue(context, canvas, slot, canvas.info, canvas.damage);
    }
    return canvas;
}

recording_canvas render(sg_composite& composite) {
    recording_canvas canvas;
    composite.draw(canvas, agg::trans_affine(2, 0, 0, 3, 7, 11));
    return canvas;
}

void order(const recording_canvas& canvas, std::initializer_list<int> ids) {
    require(canvas.paints.size() == ids.size(), "wrong paint count");
    size_t i = 0;
    for (int id : ids) require(canvas.paints[i++].id == id, "painter order changed");
}

void same_paints(const recording_canvas& a, const recording_canvas& b) {
    require(a.paints.size() == b.paints.size(), "copy/move changed paint count");
    for (size_t i = 0; i < a.paints.size(); ++i) {
        require(a.paints[i].id == b.paints[i].id && a.paints[i].mode == b.paints[i].mode,
                "copy/move changed paint order or style");
        same_rect(a.paints[i].bounds, b.paints[i].bounds, "paint placement changed");
    }
}

void bounds(plot& p, const agg::rect_d& expected) {
    agg::rect_d actual;
    p.get_bounding_rect(actual);
    same_rect(actual, expected, "stale auto-limit bounds");
}

void storage() {
    counts c;
    {
        sg_drawable_list list;
        list.add(std::make_unique<sg_element>(element(c, 1)));
        bool rejected = false;
        try { list.append(std::move(list)); }
        catch (const std::invalid_argument&) { rejected = true; }
        require(rejected && list.size() == 1 && c.live() == 1,
                "self-append must preserve ownership and reject aliasing");
    }
    {
        plot pending(0);
        pending.add(new counted_box(c, {1, 2, 3, 4}), ink(1), 0, ink(1), fill);
        pending.clear_drawing_queue();
        require(c.live() == 0, "clearing pending raw geometry leaked");
        pending.add(std::make_unique<sg_element>(element(c, 2)));
    }
    require(c.live() == 0 && c.copies == 0, "pending destruction leaked or cloned");
    {
        plot source(0);
        source.set_limits({0, 0, 10, 10});
        source.add(std::make_unique<sg_element>(element(c, 1, {1, 2, 3, 4}, fill | crisp,
                                                      agg::trans_affine(2, 0, 0, 1, 1, 1))));
        source.add(element(c, 2, {2, 1, 4, 3}, outline));
        const auto pending = render(source, true);
        order(pending, {1, 2});
        require(pending.damage.is_defined() && pending.painted.is_defined(), "missing queue bounds");
        same_rect(pending.damage.rect(), pending.painted.rect(), "queue paint bounds differ");
        const auto& m = pending.info.active_area;
        same_rect(pending.paints[0].bounds,
                  {m.tx + .3 * m.sx, m.ty + .3 * m.sy, m.tx + .7 * m.sx, m.ty + .5 * m.sy},
                  "local and plot transforms composed incorrectly");
        require(pending.paints[0].mode == 1 && pending.paints[1].mode == 2, "paint style lost");
        order(render(source), {});
        plot copy(source);
        require(c.live() == 4 && c.copies == 2, "pending copy is not deep");
        source.clear_drawing_queue();
        same_paints(pending, render(copy, true));
        plot moved(std::move(copy));
        require(c.live() == 2 && c.copies == 2, "pending move cloned or leaked");
        same_paints(pending, render(moved, true));
        moved.push_drawing_queue();
        moved.clear_drawing_queue();
        require(c.live() == 2 && c.copies == 2, "queue transfer did not retain ownership");
        order(render(moved, true), {});
        same_paints(pending, render(moved));
        require(moved.push_layer(), "push layer failed");
        moved.add(element(c, 3)); moved.commit_pending_draw();
        moved.add(element(c, 4));
        const auto committed = render(moved), queued = render(moved, true);
        order(committed, {1, 2, 3}); order(queued, {4});
        plot snapshot(moved);
        require(c.live() == 8 && c.copies == 6, "mixed layer/queue copy is not deep");
        moved.clear_current_layer();
        require(moved.pop_layer(), "pop layer failed");
        moved.clear_current_layer();
        require(c.live() == 4, "source clear affected its copy");
        plot final(std::move(snapshot));
        require(c.copies == 6 && final.current_layer_index() == 2, "committed move lost layers");
        same_paints(committed, render(final)); same_paints(queued, render(final, true));
        snapshot.add(element(c, 9)); snapshot.clear_current_layer();
        require(c.live() == 4, "moved-from plot cannot be reused");
        final.commit_pending_draw(); final.commit_pending_draw();
        order(render(final), {1, 2, 3, 4});
        final.clear_current_layer();
        order(render(final), {1, 2});
        require(c.live() == 2, "clear did not destroy committed and pending ownership");
    }
    require(c.live() == 0, "plot destruction leaked committed geometry");
}

void layers() {
    counts c;
    {
        plot p(plot::auto_limits);
        bounds(p, {0, 0, 0, 0});
        p.add(element(c, 1)); bounds(p, {1, 2, 3, 4});
        require(p.push_layer(), "auto-limit push failed");
        p.add(element(c, 2, {-5, -6, 8, 9})); bounds(p, {-5, -6, 8, 9});
        p.commit_pending_draw();
        require(p.push_layer(), "nested auto-limit push failed");
        p.add(element(c, 3, {-10, -20, 20, 30})); bounds(p, {-10, -20, 20, 30});
        {
            plot copy(p);
            copy.clear_current_layer(); bounds(copy, {-5, -6, 8, 9});
            require(copy.pop_layer(), "copied layer pop failed");
            copy.clear_current_layer(); bounds(copy, {1, 2, 3, 4});
            bounds(p, {-10, -20, 20, 30});
        }
        p.clear_current_layer(); bounds(p, {-5, -6, 8, 9});
        require(c.live() == 2, "clear pending layer leaked");
        p.add(element(c, 3, {-10, -20, 20, 30})); p.commit_pending_draw();
        bounds(p, {-10, -20, 20, 30});
        require(p.pop_layer(), "enlarged layer pop failed"); bounds(p, {-5, -6, 8, 9});
        p.clear_current_layer(); bounds(p, {1, 2, 3, 4});
        p.add(element(c, 4, {1.5, 2.5, 2.5, 3.5})); p.commit_pending_draw();
        require(p.pop_layer(), "contained layer pop failed"); bounds(p, {1, 2, 3, 4});
        p.clear_current_layer(); bounds(p, {0, 0, 0, 0});
        p.add(element(c, 5, {40, 50, 60, 70})); bounds(p, {40, 50, 60, 70});
        require(p.push_layer() && p.pop_layer(), "empty layer push/pop failed");
        bounds(p, {40, 50, 60, 70});
        p.clear_current_layer(); bounds(p, {0, 0, 0, 0});
    }
    require(c.live() == 0, "auto-limit layer changes leaked");
    const int copies = c.copies;
    {
        plot p(0);
        p.set_limits({0, 0, 10, 10});
        for (int i = 1; i < 8; ++i) {
            p.add(element(c, i));
            require(p.push_layer() && p.current_layer_index() == i + 1, "layer index changed");
        }
        p.add(element(c, 8));
        require(!p.push_layer() && p.current_layer_index() == 8, "maximum layer limit changed");
        order(render(p), {1, 2, 3, 4, 5, 6, 7}); order(render(p, true), {8});
        require(c.live() == 8 && c.copies == copies, "layer growth copied or lost ownership");
        for (int i = 7; i >= 1; --i) {
            require(p.pop_layer() && p.current_layer_index() == i && c.live() == i,
                    "pop did not destroy exactly the current layer and queue");
        }
        p.add(element(c, 9));
        require(!p.pop_layer(), "base layer was removed");
        order(render(p), {1}); order(render(p, true), {9});
        p.clear_current_layer();
        order(render(p), {}); order(render(p, true), {});
    }
    require(c.live() == 0, "layer limit/clear tests leaked");
}

void composites() {
    counts c;
    {
        sg_composite survivor;
        recording_canvas expected;
        {
            sg_composite source;
            source.add(element(c, 1, {1, 2, 3, 4}, fill | crisp));
            source.add_stroke(new counted_box(c, {2, 3, 4, 5}), ink(2), 2, outline);
            expected = render(source); order(expected, {1, 2});
            same_rect(expected.paints[0].bounds, {9, 17, 13, 23}, "composite placement changed");
            sg_composite copy(source);
            sg_composite moved(std::move(copy));
            require(c.live() == 4 && c.copies == 2, "composite copy/move ownership failed");
            same_paints(expected, render(moved));
            sg_composite assigned;
            assigned.add_fill(new counted_box(c, {0, 0, 1, 1}), ink(9));
            assigned = moved;
            require(c.live() == 6 && c.copies == 4, "composite assignment leaked old geometry");
            survivor = std::move(assigned);
            require(c.live() == 6 && c.copies == 4, "composite move assignment cloned");
        }
        require(c.live() == 2, "composite destruction invalidated survivor");
        same_paints(expected, render(survivor));
    }
    require(c.live() == 0, "composite destruction leaked");
}

template <class Function>
void fails_to_clone(Function function) {
    try { function(); } catch (const clone_failure&) { return; }
    throw std::runtime_error("clone exception was not propagated");
}

void clone_exceptions() {
    counts c;
    {
        plot source(0);
        source.set_limits({0, 0, 10, 10});
        source.add(element(c, 1)); source.add(element(c, 2));
        require(source.push_layer(), "exception-test layer push failed");
        source.add(element(c, 3)); source.commit_pending_draw();
        source.add(element(c, 4)); source.add(element(c, 5));
        const auto committed = render(source), pending = render(source, true);
        // Fail at every clone position, including after the queue has been copied.
        for (int i = 0; i < 5; ++i) {
            c.fail_after = i;
            const int copies = c.copies;
            fails_to_clone([&] { plot copy(source); });
            require(c.live() == 5 && c.copies == copies + i, "failed plot copy leaked partial clones");
            same_paints(committed, render(source)); same_paints(pending, render(source, true));
        }
        sg_composite original, target;
        original.add(element(c, 6)); original.add(element(c, 7)); target.add(element(c, 8));
        const auto old_target = render(target), old_original = render(original);
        for (int i = 0; i < 2; ++i) {
            c.fail_after = i;
            const int copies = c.copies;
            fails_to_clone([&] { target = original; });
            require(c.live() == 8 && c.copies == copies + i, "failed assignment leaked partial clones");
            same_paints(old_target, render(target)); same_paints(old_original, render(original));
        }
        c.fail_after = -1;
        plot recovered(source);
        same_paints(committed, render(recovered)); same_paints(pending, render(recovered, true));
        target = original;
        same_paints(old_original, render(target));
    }
    require(c.live() == 0, "clone exception recovery leaked geometry");
}

} // namespace

int main() {
    try {
        storage();
        layers();
        composites();
        clone_exceptions();
    } catch (const std::exception& error) {
        std::fprintf(stderr, "plot storage regression: %s\n", error.what());
        return 1;
    } catch (...) {
        std::fprintf(stderr, "plot storage regression: unexpected exception\n");
        return 1;
    }
    return 0;
}

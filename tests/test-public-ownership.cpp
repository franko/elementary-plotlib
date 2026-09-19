#include "elem/elem_cpp.h"
#include "canvas_path.h"

#include <cstdio>
#include <stdexcept>
#include <utility>

namespace {

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

struct injected_failure {};
struct counts {
    int created = 0, destroyed = 0, copies = 0, fail_after = -1;
    bool fail_bounds = false;
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
        if (count.fail_after == 0) throw injected_failure{};
        if (count.fail_after > 0) --count.fail_after;
        auto result = new counted_path(count);
        ++count.copies;
        return result;
    }
    void bounding_box(double* x1, double* y1, double* x2, double* y2) override {
        if (count.fail_bounds) throw injected_failure{};
        elem_path::bounding_box(x1, y1, x2, y2);
    }
};

struct tracked_object : elem::Object {
    using Object::operator=;
    explicit tracked_object(counts& c) : Object(new counted_path(c)) {}
    elem_object* get() const { return object_impl_; }
};

template <class Function>
void expect_failure(Function function, const char* message) {
    try { function(); } catch (const injected_failure&) { return; }
    throw std::runtime_error(message);
}

} // namespace

// Link with libelemwin_dep for backend symbols, but never initialize the GUI.
int main() {
    using namespace elem;
    try {
        counts objects, old_object;
        {
            tracked_object source(objects), destination(old_object);
            auto original = destination.get();
            objects.fail_after = 0;
            expect_failure([&] { destination = source; }, "Object assignment swallowed copy failure");
            require(destination.get() == original && old_object.live() == 1 && objects.live() == 1,
                    "failed Object assignment changed destination or leaked");
            objects.fail_after = -1;
            destination = source;
            require(destination.get() != source.get() && old_object.live() == 0 && objects.live() == 2,
                    "Object assignment did not replace with an independent copy");

            Object moved(std::move(source));
            require(source.get() == nullptr, "Object move did not release source");
            Object empty, empty_copy(empty), moved_copy(source);
            destination = source;
            require(destination.get() == nullptr && objects.live() == 1,
                    "assignment from moved-from Object did not clear destination");
            destination = empty;
            Plot plot;
            plot.Add(empty, color::Red, 1, color::None);
            plot.Add(empty_copy, color::Red, 1, color::None);
            plot.Add(moved_copy, color::Red, 1, color::None);
            plot.AddStroke(source, color::Red, 1);
            require(objects.live() == 1 && objects.copies == 1, "empty Add changed ownership");
            plot.Add(std::move(moved), color::Red, 1, color::None);
            require(objects.live() == 1 && objects.copies == 1, "Add cloned or lost transferred Object");
            plot.ClearLayer();
            require(objects.live() == 0, "ClearLayer did not release transferred Object");
        }
        require(objects.live() == 0 && old_object.live() == 0, "Object lifetime leaked");

        counts bounds;
        {
            Plot plot; // Default AutoLimits forces bounding_box() during Add.
            tracked_object object(bounds);
            bounds.fail_bounds = true;
            expect_failure([&] { plot.Add(std::move(object), color::Blue, 1, color::None); },
                           "Add swallowed autoscale bounds failure");
            require(object.get() == nullptr && bounds.created == 1 && bounds.destroyed == 1 &&
                    bounds.copies == 0, "failed Add leaked or retained transferred ownership");
            bounds.fail_bounds = false;
            plot.AddStroke(tracked_object(bounds), color::Blue, 1);
            require(bounds.live() == 1, "plot unusable after failed Add");
            plot.ClearLayer();
        }
        require(bounds.live() == 0, "failed Add recovery leaked");

        counts clones, old_plot;
        {
            Plot source, destination;
            source.AddStroke(tracked_object(clones), color::Red, 1);
            source.AddStroke(tracked_object(clones), color::Blue, 1);
            destination.AddStroke(tracked_object(old_plot), color::Black, 1);
            clones.fail_after = 1; // Throw only after one replacement clone exists.
            expect_failure([&] { destination = source; }, "Plot assignment swallowed copy failure");
            require(clones.copies == 1 && clones.live() == 2 && clones.destroyed == 1 &&
                    old_plot.live() == 1, "failed Plot assignment lost old plot or leaked partial clone");
            {
                Plot preserved(destination);
                require(old_plot.live() == 2 && old_plot.copies == 1,
                        "failed Plot assignment changed destination contents");
            }
            clones.fail_after = -1;
            destination = source;
            require(old_plot.live() == 0 && clones.live() == 4 && clones.copies == 3,
                    "Plot assignment recovery did not replace old contents");
            source.ClearLayer();
            require(clones.live() == 2, "source clear affected assigned Plot");
            Plot copy(destination);
            require(clones.live() == 4 && clones.copies == 5, "Plot copy is not independent");
        }
        require(clones.live() == 0 && old_plot.live() == 0, "Plot lifetime leaked");

        for (int symbol : {5, 6}) { // Public plus/cross symbols contain owning stroke adapters.
            Plot survivor;
            {
                Markers markers(6, MarkerSymbol(symbol));
                markers.MoveTo(1, 2); markers.LineTo(3, 4);
                Markers copied(markers);
                Plot source;
                source.AddStroke(markers, color::Red, 1);
                source.Add(copied, color::Blue, 1, color::None);
                Plot copy(source);
                source.ClearLayer();
                survivor = copy;
            }
            Plot copy(survivor);
            survivor.ClearLayer();
            copy.CommitPendingDraw();
            copy.ClearLayer();
        }
    } catch (const std::exception& error) {
        std::fprintf(stderr, "public ownership regression: %s\n", error.what());
        return 1;
    } catch (...) {
        std::fprintf(stderr, "public ownership regression: unexpected exception\n");
        return 1;
    }
    return 0;
}

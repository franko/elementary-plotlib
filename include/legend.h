#pragma once

#include <limits>
#include <vector>

#include "sg_group.h"

namespace graphics {

enum class legend_direction {
    horizontal,
    vertical,
};

enum class legend_corner {
    top_left,
    top_right,
    bottom_left,
    bottom_right,
};

enum class legend_edge {
    top,
    bottom,
    left,
    right,
};

struct legend_size {
    double width = 0.0;
    double height = 0.0;
};

/* A legend owns independently measurable group entries. Entry geometry is
   measured once, while placement is resolved for the current plot rectangle
   whenever the legend is drawn. */
class legend {
public:
    legend() = default;
    legend(const legend&) = default;
    legend(legend&&) noexcept = default;
    legend& operator=(const legend&) = default;
    legend& operator=(legend&&) noexcept = default;

    void add(sg_group entry);

    bool empty() const {
        return m_entries.empty();
    }

    legend_size measure(
        legend_direction direction,
        double max_width = std::numeric_limits<double>::infinity()) const;

    void draw_outside(virtual_canvas& canvas, const agg::trans_affine& area,
                      legend_direction direction,
                      opt_rect<double>* painted_bounds = nullptr);

    void draw_inside(virtual_canvas& canvas, const agg::trans_affine& area,
                     legend_corner corner,
                     opt_rect<double>* painted_bounds = nullptr);

    void draw_inside_edge(virtual_canvas& canvas, const agg::trans_affine& area,
                          legend_edge edge,
                          opt_rect<double>* painted_bounds = nullptr);

private:
    struct entry {
        sg_group group;
        agg::rect_d bounds;
    };

    struct row {
        std::vector<size_t> entries;
        double width = 0.0;
        double height = 0.0;
    };

    struct layout {
        std::vector<row> rows;
        double width = 0.0;
        double height = 0.0;
    };

    layout make_layout(legend_direction direction, double max_width) const;
    void draw_entry(entry& item, virtual_canvas& canvas, double x, double y,
                    opt_rect<double>* painted_bounds);

    std::vector<entry> m_entries;
};

} // namespace graphics

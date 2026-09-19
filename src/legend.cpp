#include "legend.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace graphics {

namespace {

constexpr double legend_padding = 8.0;
constexpr double legend_gap = 8.0;

} // namespace

void legend::add(sg_group entry_group)
{
    const opt_rect<double> measured = entry_group.screen_bounds();
    if (!measured.is_defined()) {
        return;
    }

    m_entries.push_back(entry{std::move(entry_group), measured.rect()});
}

legend::layout legend::make_layout(legend_direction direction, double max_width) const
{
    layout result;
    if (m_entries.empty()) {
        return result;
    }

    if (direction == legend_direction::vertical) {
        result.rows.reserve(m_entries.size());
        for (size_t i = 0; i < m_entries.size(); ++i) {
            const agg::rect_d& bounds = m_entries[i].bounds;
            row current;
            current.entries.push_back(i);
            current.width = bounds.x2 - bounds.x1;
            current.height = bounds.y2 - bounds.y1;
            result.width = std::max(result.width, current.width);
            result.height += current.height;
            if (i != 0) {
                result.height += legend_gap;
            }
            result.rows.push_back(std::move(current));
        }
        return result;
    }

    const bool wrap = std::isfinite(max_width);
    row current;
    for (size_t i = 0; i < m_entries.size(); ++i) {
        const agg::rect_d& bounds = m_entries[i].bounds;
        const double width = bounds.x2 - bounds.x1;
        const double height = bounds.y2 - bounds.y1;
        const double proposed_width = current.entries.empty()
            ? width
            : current.width + legend_gap + width;

        if (!current.entries.empty() && wrap && proposed_width > max_width) {
            result.width = std::max(result.width, current.width);
            result.height += current.height;
            if (!result.rows.empty()) {
                result.height += legend_gap;
            }
            result.rows.push_back(std::move(current));
            current = row{};
        }

        current.entries.push_back(i);
        current.width = current.entries.size() == 1
            ? width
            : current.width + legend_gap + width;
        current.height = std::max(current.height, height);
    }

    if (!current.entries.empty()) {
        result.width = std::max(result.width, current.width);
        result.height += current.height;
        if (!result.rows.empty()) {
            result.height += legend_gap;
        }
        result.rows.push_back(std::move(current));
    }
    return result;
}

legend_size legend::measure(legend_direction direction, double max_width) const
{
    const layout measured = make_layout(direction, max_width);
    return legend_size{measured.width, measured.height};
}

void legend::draw_entry(entry& item, virtual_canvas& canvas, double x, double y,
                        opt_rect<double>* painted_bounds)
{
    const agg::rect_d& bounds = item.bounds;
    const agg::trans_affine placement = agg::trans_affine_translation(
        x - bounds.x1, y - bounds.y1);
    opt_rect<double> entry_bounds;
    item.group.draw(canvas, placement, painted_bounds ? &entry_bounds : nullptr);
    if (painted_bounds) {
        painted_bounds->add<rect_union>(entry_bounds);
    }
}

void legend::draw_outside(virtual_canvas& canvas, const agg::trans_affine& area,
                          legend_direction direction,
                          opt_rect<double>* painted_bounds)
{
    const double available_width = direction == legend_direction::horizontal
        ? std::max(0.0, area.sx - 2.0 * legend_padding)
        : std::numeric_limits<double>::infinity();
    const layout positioned = make_layout(direction, available_width);
    if (positioned.rows.empty()) {
        return;
    }

    const double content_width = std::max(0.0, area.sx - 2.0 * legend_padding);
    if (direction == legend_direction::horizontal) {
        double y = area.ty + legend_padding;
        for (const row& current : positioned.rows) {
            double x = area.tx + legend_padding +
                       (content_width - current.width) / 2.0;
            for (size_t index : current.entries) {
                entry& item = m_entries[index];
                const double height = item.bounds.y2 - item.bounds.y1;
                draw_entry(item, canvas, x,
                           y + (current.height - height) / 2.0,
                           painted_bounds);
                x += item.bounds.x2 - item.bounds.x1 + legend_gap;
            }
            y += current.height + legend_gap;
        }
        return;
    }

    double y = area.ty + area.sy - legend_padding;
    for (const row& current : positioned.rows) {
        entry& item = m_entries[current.entries.front()];
        const double height = item.bounds.y2 - item.bounds.y1;
        y -= height;
        draw_entry(item, canvas,
                   area.tx + legend_padding,
                   y, painted_bounds);
        y -= legend_gap;
    }
}

void legend::draw_inside(virtual_canvas& canvas, const agg::trans_affine& area,
                         legend_corner corner,
                         opt_rect<double>* painted_bounds)
{
    const layout positioned = make_layout(legend_direction::vertical,
                                          std::numeric_limits<double>::infinity());
    if (positioned.rows.empty()) {
        return;
    }

    double x = area.tx + legend_padding;
    double y;
    switch (corner) {
    case legend_corner::top_left:
        y = area.ty + area.sy - legend_padding -
            positioned.rows.front().height;
        break;
    case legend_corner::top_right:
        x = area.tx + area.sx - legend_padding - positioned.width;
        y = area.ty + area.sy - legend_padding -
            positioned.rows.front().height;
        break;
    case legend_corner::bottom_left:
        y = area.ty + legend_padding + positioned.height -
            positioned.rows.front().height;
        break;
    case legend_corner::bottom_right:
        x = area.tx + area.sx - legend_padding - positioned.width;
        y = area.ty + legend_padding + positioned.height -
            positioned.rows.front().height;
        break;
    }

    for (const row& current : positioned.rows) {
        entry& item = m_entries[current.entries.front()];
        draw_entry(item, canvas,
                   x, y,
                   painted_bounds);
        y -= current.height + legend_gap;
    }
}

void legend::draw_inside_edge(virtual_canvas& canvas,
                              const agg::trans_affine& area,
                              legend_edge edge,
                              opt_rect<double>* painted_bounds)
{
    const bool horizontal = edge == legend_edge::top || edge == legend_edge::bottom;
    const legend_direction direction = horizontal
        ? legend_direction::horizontal
        : legend_direction::vertical;
    const double max_width = horizontal
        ? std::max(0.0, area.sx - 2.0 * legend_padding)
        : std::numeric_limits<double>::infinity();
    const layout positioned = make_layout(direction, max_width);
    if (positioned.rows.empty()) {
        return;
    }

    if (horizontal) {
        const double content_width = std::max(0.0, area.sx - 2.0 * legend_padding);
        if (edge == legend_edge::top) {
            double y = area.ty + area.sy - legend_padding;
            for (const row& current : positioned.rows) {
                y -= current.height;
                double x = area.tx + legend_padding +
                           (content_width - current.width) / 2.0;
                for (size_t index : current.entries) {
                    entry& item = m_entries[index];
                    const double height = item.bounds.y2 - item.bounds.y1;
                    draw_entry(item, canvas, x,
                               y + (current.height - height) / 2.0,
                               painted_bounds);
                    x += item.bounds.x2 - item.bounds.x1 + legend_gap;
                }
                y -= legend_gap;
            }
        } else {
            double y = area.ty + legend_padding;
            for (const row& current : positioned.rows) {
                double x = area.tx + legend_padding +
                           (content_width - current.width) / 2.0;
                for (size_t index : current.entries) {
                    entry& item = m_entries[index];
                    const double height = item.bounds.y2 - item.bounds.y1;
                    draw_entry(item, canvas, x,
                               y + (current.height - height) / 2.0,
                               painted_bounds);
                    x += item.bounds.x2 - item.bounds.x1 + legend_gap;
                }
                y += current.height + legend_gap;
            }
        }
        return;
    }

    const double x = edge == legend_edge::left
        ? area.tx + legend_padding
        : area.tx + area.sx - legend_padding - positioned.width;
    double y = area.ty + (area.sy + positioned.height) / 2.0 -
               positioned.rows.front().height;
    for (const row& current : positioned.rows) {
        entry& item = m_entries[current.entries.front()];
        draw_entry(item, canvas, x, y, painted_bounds);
        y -= current.height + legend_gap;
    }
}

} // namespace graphics

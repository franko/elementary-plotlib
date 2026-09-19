#pragma once

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <vector>

#include "agg_trans_affine.h"

#include "canvas.h"
#include "rect.h"

// Placements must be usable without inversion. Singular transforms are legal:
// geometry is then allowed to collapse to a line or to a point.
inline bool affine_is_finite(const agg::trans_affine& m) {
    return std::isfinite(m.sx) && std::isfinite(m.shy) && std::isfinite(m.shx) &&
           std::isfinite(m.sy) && std::isfinite(m.tx) && std::isfinite(m.ty);
}

inline void check_finite_affine(const agg::trans_affine& m) {
    if (!affine_is_finite(m)) {
        throw std::invalid_argument("transform coefficients must be finite");
    }
}

// Conservative parent-space bounds of a model box under an affine placement.
// Transforming an axis-aligned box is not tight for rotations and shears.
inline opt_rect<double> transform_bounds(const agg::rect_d& box, const agg::trans_affine& m) {
    opt_rect<double> result;
    if (!box.is_valid()) {
        return result;
    }
    const double xs[] = {box.x1, box.x2, box.x2, box.x1};
    const double ys[] = {box.y1, box.y1, box.y2, box.y2};
    for (unsigned i = 0; i < 4; ++i) {
        double x = xs[i], y = ys[i];
        m.transform(&x, &y);
        if (!std::isfinite(x) || !std::isfinite(y)) {
            return opt_rect<double>();
        }
        result.add<rect_union>(agg::rect_d(x, y, x, y));
    }
    return result;
}

// Largest singular value of the linear part: the maximum stretch a curve can
// undergo. agg::trans_affine::scale() averages instead, and underestimates it
// under anisotropic scaling and shear. The two agree for similarities.
inline double affine_max_stretch(const agg::trans_affine& m) {
    const double e = (m.sx + m.sy) / 2, f = (m.sx - m.sy) / 2;
    const double g = (m.shy + m.shx) / 2, h = (m.shy - m.shx) / 2;
    return std::sqrt(e * e + h * h) + std::sqrt(f * f + g * g);
}

// Tessellation factor to pass to elem_object::apply_transform, whose users
// multiply it by agg::trans_affine::scale(). The product is the maximum
// stretch, so similarity placements keep their existing approximation scale.
inline double approximation_scale_of(const agg::trans_affine& m) {
    const double average = m.scale();
    const double stretch = affine_max_stretch(m);
    if (!(average > 0.0) || !std::isfinite(average) || !std::isfinite(stretch)) {
        return 1.0;
    }
    return stretch / average;
}

// A drawing operation, rather than a single vertex source. Coordinates and
// data bounds are in the parent's space; painted bounds are in logical pixels.
class sg_drawable {
public:
    virtual ~sg_drawable() = default;
    virtual std::unique_ptr<sg_drawable> clone() const = 0;
    virtual opt_rect<double> data_bounds() const = 0;
    virtual void draw(virtual_canvas& canvas, const agg::trans_affine& parent_to_screen,
                      opt_rect<double>* painted_bounds = nullptr) = 0;
};

// Deep-copy values, transfer ownership on insertion, and preserve painter order.
class sg_drawable_list {
public:
    sg_drawable_list() = default;
    sg_drawable_list(sg_drawable_list&&) noexcept = default;
    sg_drawable_list(const sg_drawable_list& source) {
        m_items.reserve(source.size());
        for (const auto& item : source.m_items) {
            add(item->clone());
        }
    }

    sg_drawable_list& operator=(sg_drawable_list source) {
        m_items.swap(source.m_items);
        return *this;
    }

    void add(std::unique_ptr<sg_drawable> item) {
        if (!item) {
            throw std::logic_error("drawable must not be null");
        }
        m_items.push_back(std::move(item));
    }

    void append(sg_drawable_list&& source) {
        if (this == &source) {
            throw std::invalid_argument("cannot append a drawable list to itself");
        }
        if (source.size() > m_items.max_size() - size()) {
            throw std::length_error("drawable list is too large");
        }
        const size_t required = size() + source.size();
        if (required > m_items.capacity()) {
            const size_t growth = std::min(m_items.capacity(), m_items.max_size() - m_items.capacity());
            m_items.reserve(std::max(required, m_items.capacity() + growth));
        }
        for (auto& item : source.m_items) {
            m_items.push_back(std::move(item));
        }
        source.clear();
    }

    void clear() { m_items.clear(); }
    size_t size() const { return m_items.size(); }
    auto begin() { return m_items.begin(); }
    auto end() { return m_items.end(); }
    auto begin() const { return m_items.begin(); }
    auto end() const { return m_items.end(); }

private:
    std::vector<std::unique_ptr<sg_drawable>> m_items;
};

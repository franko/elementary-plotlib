#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

#include "canvas.h"
#include "rect.h"

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

#pragma once

#include <initializer_list>
#include <utility>

#include "sg_path.h"
#include "sg_object.h"
#include "trans.h"

namespace graphics {

class path : public graphics::transform::scaling_a {
public:
    path() : graphics::transform::scaling_a(&m_path), m_path() { }

    path(std::initializer_list<std::pair<double, double>> lst): graphics::transform::scaling_a(&m_path), m_path() {
        bool first = true;
        auto& p = m_path.self();
        for (const auto& v : lst) {
            if (first) {
                p.move_to(v.first, v.second);
                first = false;
            } else {
                p.line_to(v.first, v.second);
            }
        }
    }

    void line_to(double x, double y) {
        m_path.self().line_to(x, y);
    }

    void move_to(double x, double y) {
        m_path.self().move_to(x, y);
    }

    void close_polygon() {
        m_path.self().close_polygon();
    }

    virtual sg_object *copy() const {
        path *new_object = new path();
        m_path.copy_content(new_object->m_path);
        return new_object;
    }

private:
    sg_path m_path;
};

class polygon : public path {
public:
    polygon(): path() { }
    polygon(std::initializer_list<std::pair<double, double>> lst): path(lst) {
        close_polygon();
    }
};

class markers : public graphics::transform::marker_a {
public:
    markers(double size, sg_object* sym): graphics::transform::marker_a(&m_scaling_path, size, sym), m_scaling_path(&m_path) { }

    void line_to(double x, double y) {
        m_path.self().line_to(x, y);
    }

    void move_to(double x, double y) {
        m_path.self().move_to(x, y);
    }

    void close_polygon() {
        m_path.self().close_polygon();
    }

private:
    graphics::transform::scaling_a m_scaling_path;
    sg_path m_path;
};

}

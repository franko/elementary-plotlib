#ifndef AGGPLOT_PATH_H
#define AGGPLOT_PATH_H

#include "sg_path.h"
#include "sg_element.h"

namespace graphics {

class path : public graphics::transform::scaling_a {
public:
    path() : graphics::transform::scaling_a(&m_path), m_path() { }

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
    sg_path m_path;
};

}

#endif

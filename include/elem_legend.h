#pragma once

#include <utility>

#include "elem_group.h"
#include "legend.h"

/* Opaque C handle used by both the C API and the public C++ Legend value. */
class elem_legend {
public:
    elem_legend() = default;
    elem_legend(const elem_legend&) = default;
    elem_legend(elem_legend&&) noexcept = default;
    elem_legend& operator=(const elem_legend&) = default;
    elem_legend& operator=(elem_legend&&) noexcept = default;

    void add(elem_group entry) {
        m_value.add(sg_group(std::move(entry)));
    }

    graphics::legend& value() {
        return m_value;
    }

private:
    graphics::legend m_value;
};

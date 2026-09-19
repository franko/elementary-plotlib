#pragma once

#include "sg_group.h"

/* Opaque handle behind the public elem_group C type and the C++ Group class.
   A group handle is a value: it is deep-copied, moved, and freed explicitly,
   and it is never an elem_object. Geometry casts do not extend to it because a
   group is not a vertex source. */
class elem_group : public sg_group {
public:
    using sg_group::sg_group;

    elem_group() = default;
    elem_group(const sg_group& source) : sg_group(source) { }
    elem_group(sg_group&& source) noexcept : sg_group(std::move(source)) { }
};

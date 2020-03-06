#pragma once

#include "elem_window_factory.h"

class native_elem_window_factory : public elem_window_factory {
public:
    elem_window *create() override;
    elem_window *create_with_layout(const char *layout) override;
};

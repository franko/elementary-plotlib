#pragma once

class elem_window;

class elem_window_factory {
public:
    virtual elem_window *create() = 0;
    virtual elem_window *create_with_layout(const char *layout) = 0;
    virtual ~elem_window_factory() { }
};

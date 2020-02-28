#pragma once

class window_close_callback {
public:
    virtual void execute() = 0;
    virtual ~window_close_callback() { }
};

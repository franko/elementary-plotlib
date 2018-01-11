#ifndef XWINDOW_X_CONNECTION_H
#define XWINDOW_X_CONNECTION_H

#include <X11/Xlib.h>

#include "defs.h"

struct x_connection {
    Display*             display;
    int                  screen;
    int                  depth;
    Visual*              visual;

    x_connection() : display(0) {};
    ~x_connection() {
        this->close();
    };

    bool init();
    void close();
};

#endif

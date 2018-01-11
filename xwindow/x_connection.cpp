#include "x_connection.h"
#include "fatal.h"

bool x_connection::init()
{
    display = XOpenDisplay(NULL);
    if(display == 0)
    {
        fatal_exception("Unable to open DISPLAY!\n");
    }

    screen = XDefaultScreen(display);
    depth  = XDefaultDepth (display, screen);
    visual = XDefaultVisual(display, screen);

    return true;
}

void x_connection::close()
{
    if (display)
    {
        XCloseDisplay(display);
        display = 0;
    }
}

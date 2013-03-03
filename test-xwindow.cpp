#include "xwindow.h"

struct dummy_renderer : render_target {
    virtual void resize(unsigned width, unsigned height)
    {
        fprintf(stderr, "resize: %u, %u\n", width, height);
    }

    virtual void draw()
    {
        fprintf(stderr, "draw\n");
    }
};

int main()
{
    dummy_renderer ren;
    xwindow* win = new xwindow(ren, agg::pix_format_bgr24, true);
    win->init(640, 480, xwindow::window_resize);
    win->run();
    return 0;
}

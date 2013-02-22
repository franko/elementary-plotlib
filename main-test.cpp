#include "window_surface.h"

class dummy_window : public display_window {
public:
    dummy_window(): m_surf(0) {}

    vod attach(window_surface* surf) { m_surf = surf; }

    virtual void update_region(const agg::rect_i& r)
    {
        printf("updating x1, y1, x2, y2.\n", r.x1, r.y1, r.x2, r.y2);
    }

private:
    window_surface* m_surf;
};

int main()
{
    dummy_window win;
    window_surface surf(&win, "h..");
    win.attach(&surf);
}

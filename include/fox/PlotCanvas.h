#pragma once

#include <fx.h>

#include "render_config.h"
#include "window_surface.h"
#include "status_notifier.h"
#include "window_flags.h"

class fxcanvas_window : public graphics::display_window {
public:
    fxcanvas_window(graphics::render_target& tgt);
    ~fxcanvas_window();

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);
    virtual void lock()   { m_mutex.lock();   }
    virtual void unlock() { m_mutex.unlock(); }
    virtual int status() { return m_window_status.value(); }

    int set_notify_request(notify_request<graphics::window_status_e>& request) {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_window_status.set_notify_request(request);
    }

private:
    std::mutex m_mutex;
    status_notifier<graphics::window_status_e> m_window_status;
    graphics::render_target& m_target;
};

class PlotCanvas : public FXCanvas {
    FXDECLARE(PlotCanvas)
public:
    PlotCanvas(FXComposite* p, FXObject* tgt=nullptr, FXSelector sel=0, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0) :
        FXCanvas(p, tgt, sel, opts, x, y, w, h)
    { }

    ~PlotCanvas() { }

private:
    fxcanvas_window m_window;
};

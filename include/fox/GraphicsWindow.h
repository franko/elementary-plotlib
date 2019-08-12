#pragma once

#include <fx.h>

#include "fox/window_fox.h"

class GraphicsWindow : public FXWindow {
    FXDECLARE(GraphicsWindow)
protected:
    GraphicsWindow(): m_window(this) { }
private:
    GraphicsWindow(const GraphicsWindow&);
    GraphicsWindow &operator=(const GraphicsWindow&);
public:
    GraphicsWindow(FXComposite* p, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0);

    ~GraphicsWindow();

    int attach(graphics::plot* p, const char* slot_str) {
        return m_window.attach(p, slot_str);
    }

    void slot_refresh(unsigned index) {
        m_window.slot_refresh(index);
    }

    void wait() {
        notify_request<graphics::window_status_e> req{graphics::window_closed};
        int retval = m_window.set_notify_request(req);
        if (retval == request_success) {
            req.wait();
        }
    }

    void position(FXint x, FXint y, FXint w, FXint h) override;
    void create() override;

    long onUpdateRegion (FXObject *, FXSelector, void *);
    long onPaint        (FXObject *, FXSelector, void *);
    long onMap          (FXObject *, FXSelector, void *);

    enum {
        ID_UPDATE_REGION = FXWindow::ID_LAST,
        ID_LAST
    };

private:
    window_fox m_window;
};

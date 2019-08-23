#pragma once

#include <fx.h>

// We include here only the public interface libcanvas header.
#include "libcanvas.h"

namespace libcanvas {

class GraphicsWindow : public FXWindow {
    FXDECLARE(GraphicsWindow)
protected:
    GraphicsWindow(): m_window_impl(nullptr) { }
private:
    GraphicsWindow(const GraphicsWindow&);
    GraphicsWindow &operator=(const GraphicsWindow&);
public:
    GraphicsWindow(FXComposite* p, const char *split_str = nullptr, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0);

    ~GraphicsWindow();

    int Attach(libcanvas::Plot& p, const char* slot_str);
    void SlotRefresh(unsigned index);
    void Wait();

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
    struct WindowImpl;
    WindowImpl *m_window_impl;
};
}

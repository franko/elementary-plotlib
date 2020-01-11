#pragma once

#include <fx.h>

// We include here only the public interface libcanvas header.
#include "libelplot.h"

class window_fox_kernel;

namespace FX {

class FXElemPlotWindow : public FXWindow {
    FXDECLARE(FXElemPlotWindow)
protected:
    FXElemPlotWindow(): m_window(nullptr) { }
private:
    FXElemPlotWindow(const FXElemPlotWindow&);
    FXElemPlotWindow &operator=(const FXElemPlotWindow&);
public:
    FXElemPlotWindow(FXComposite* p, window_fox_kernel *win = nullptr, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0);
    ~FXElemPlotWindow();

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
    window_fox_kernel *m_window;
};
}

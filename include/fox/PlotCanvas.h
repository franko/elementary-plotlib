#pragma once

#include <fx.h>

class window_fox;

class PlotCanvas : public FXWindow {
    FXDECLARE(PlotCanvas)
protected:
    PlotCanvas() { }
private:
    PlotCanvas(const PlotCanvas&);
    PlotCanvas &operator=(const PlotCanvas&);
public:
    PlotCanvas(FXComposite* p, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0) :
        FXWindow(p, opts, x, y, w, h)
    { }

    ~PlotCanvas() { }

    void position(FXint x, FXint y, FXint w, FXint h) override;
    void create() override;
    void destroy() override;

    void bind(window_fox *w) { m_window = w; }

    long onUpdateRegion (FXObject *, FXSelector, void *);
    long onPaint        (FXObject *, FXSelector, void *);
    long onMap          (FXObject *, FXSelector, void *);

    enum {
        // ID_CANVAS = FXWindow::ID_LAST,
        ID_UPDATE_REGION = FXWindow::ID_LAST,
        ID_LAST
    };

private:
    window_fox *m_window;
};

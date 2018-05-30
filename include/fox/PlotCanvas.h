#pragma once

#include <fx.h>

class window_fox;

class PlotCanvas : public FXCanvas {
    FXDECLARE(PlotCanvas)
protected:
    PlotCanvas() {}
public:
    PlotCanvas(FXComposite* p, FXObject* tgt=nullptr, FXSelector sel=0, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0) :
        FXCanvas(p, tgt, sel, opts, x, y, w, h)
    { }

    ~PlotCanvas() { }

    void bind(window_fox *w) { m_window = w; }

    long onUpdateRegion (FXObject *, FXSelector, void *);
    long onPaint        (FXObject *, FXSelector, void *);

    enum {
        ID_CANVAS = FXCanvas::ID_LAST,
        ID_UPDATE_REGION,
        ID_LAST
    };

private:
    window_fox *m_window;
};

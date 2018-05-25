#pragma once

#include <fx.h>

class PlotCanvas : public FXCanvas {
    FXDECLARE(PlotCanvas)
public:
    PlotCanvas(FXComposite* p, FXObject* tgt=nullptr, FXSelector sel=0, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0) :
        FXCanvas(p, tgt, sel, opts, x, y, w, h)
    { }

    ~PlotCanvas() { }

    // Messages for our class
    enum {
        ID_CANVAS=FXCanvas::ID_LAST,
        ID_LAST
    };
};

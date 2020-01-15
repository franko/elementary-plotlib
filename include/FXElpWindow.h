#pragma once

#include <fx.h>

// We include here only the public interface libcanvas header.
#include "libelplot.h"
#include "FXElemPlotWindow.h"

namespace FX {

class FXElpWindow : public FXElemPlotWindow {
    FXDECLARE(FXElpWindow)
protected:
    FXElpWindow() { }
private:
    FXElpWindow(const FXElpWindow&);
    FXElpWindow &operator=(const FXElpWindow&);
public:
    FXElpWindow(FXComposite* p, const char *split_str = nullptr, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0);

    int Attach(elp::Plot& p, const char* slot_str);
    void SetLayout(const char *fmt);
    void SlotRefresh(unsigned index);
    void Wait();

private:
    elp::Window m_window;
};
}

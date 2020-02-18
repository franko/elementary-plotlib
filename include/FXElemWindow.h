#pragma once

#include <fx.h>

// We include here only the public interface elementary plot library header.
#include "libelplot.h"
#include "FXElemBaseWindow.h"

namespace FX {

class FXElemWindow : public FXElemBaseWindow {
    FXDECLARE(FXElemWindow)
protected:
    FXElemWindow() { }
private:
    FXElemWindow(const FXElemWindow&);
    FXElemWindow &operator=(const FXElemWindow&);
public:
    FXElemWindow(FXComposite* p, const char *split_str = nullptr, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0);

    int Attach(elem::Plot& p, const char* slot_str);
    void SetLayout(const char *fmt);
    void SlotRefresh(unsigned index);
    void Wait();

private:
    elem::Window m_window;
};
}

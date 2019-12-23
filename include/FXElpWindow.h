#pragma once

#include <fx.h>

// We include here only the public interface libcanvas header.
#include "libelplot.h"

class window_fox;

namespace FX {

class FXElpWindow : public FXWindow {
    FXDECLARE(FXElpWindow)
protected:
    FXElpWindow(): m_window_impl(nullptr) { }
private:
    FXElpWindow(const FXElpWindow&);
    FXElpWindow &operator=(const FXElpWindow&);
public:
    FXElpWindow(FXComposite* p, const char *split_str = nullptr, FXuint opts=FRAME_NORMAL, FXint x=0, FXint y=0, FXint w=0, FXint h=0);

    ~FXElpWindow();

    int Attach(elp::Plot& p, const char* slot_str);
    void SetLayout(const char *fmt);
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
    window_fox *m_window_impl;
};
}

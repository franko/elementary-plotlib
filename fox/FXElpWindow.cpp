#include "FXElpWindow.h"
#include "window_platform_fox.h"

FXDEFMAP(FXElpWindow) FXElpWindowMap[] = {};

FXIMPLEMENT(FXElpWindow,FXElemBaseWindow,FXElpWindowMap,ARRAYNUMBER(FXElpWindowMap))

FXElpWindow::FXElpWindow(FXComposite* p, const char *split_str, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXElemBaseWindow(p, nullptr, opts, x, y, w, h),
    m_window(new elp_window_fox(split_str, this))
{
}

int FXElpWindow::Attach(elp::Plot& p, const char* slot_str) {
    return m_window.Attach(p, slot_str);
}

void FXElpWindow::SetLayout(const char *fmt) {
    m_window.SetLayout(fmt);
}

void FXElpWindow::SlotRefresh(unsigned index) {
    m_window.SlotRefresh(index);
}

void FXElpWindow::Wait() {
    m_window.Wait();
}

#include "FXElemWindow.h"
#include "window_platform_fox.h"

FXDEFMAP(FXElemWindow) FXElpWindowMap[] = {};

FXIMPLEMENT(FXElemWindow,FXElemBaseWindow,FXElpWindowMap,ARRAYNUMBER(FXElpWindowMap))

FXElemWindow::FXElemWindow(FXComposite* p, const char *split_str, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXElemBaseWindow(p, nullptr, opts, x, y, w, h),
    m_window(new elp_window_fox(split_str, this))
{
}

int FXElemWindow::Attach(elp::Plot& p, const char* slot_str) {
    return m_window.Attach(p, slot_str);
}

void FXElemWindow::SetLayout(const char *fmt) {
    m_window.SetLayout(fmt);
}

void FXElemWindow::SlotRefresh(unsigned index) {
    m_window.SlotRefresh(index);
}

void FXElemWindow::Wait() {
    m_window.Wait();
}

#include "FXElemBaseWindow.h"
#include "window_fox.h"

FXDEFMAP(FXElemBaseWindow) FXElemBaseWindowMap[] = {
    FXMAPFUNC(SEL_MAP, 0, FXElemBaseWindow::onMap),
    FXMAPFUNC(SEL_PAINT, 0, FXElemBaseWindow::onPaint),
    FXMAPFUNC(SEL_IO_READ, FXElemBaseWindow::ID_ELEM_UPDATE, FXElemBaseWindow::onUpdateRegion),
    FXMAPFUNC(SEL_IO_READ, FXElemBaseWindow::ID_ELEM_CLOSE, FXElemBaseWindow::onCloseRequest),
};

FXIMPLEMENT(FXElemBaseWindow,FXWindow,FXElemBaseWindowMap,ARRAYNUMBER(FXElemBaseWindowMap))

FXElemBaseWindow::FXElemBaseWindow(FXComposite* p, window_fox *win, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXWindow(p, opts, x, y, w, h), m_window(win)
{
    flags |= FLAG_SHOWN;
}

FXElemBaseWindow::~FXElemBaseWindow() {
    m_window->set_status(graphics::window_closed);
}

void FXElemBaseWindow::position(FXint x, FXint y, FXint w, FXint h) {
    if (m_window) {
        m_window->on_resize(w, h);
    }
    FXWindow::position(x, y, w, h);
}

void FXElemBaseWindow::create() {
    if (m_window) {
        m_window->set_status(graphics::window_starting);
    }
    FXWindow::create();
}

long FXElemBaseWindow::onUpdateRegion(FXObject *, FXSelector, void *) {
    if (m_window) {
        m_window->call_update_region();
    }
    return 1;
}

long FXElemBaseWindow::onCloseRequest(FXObject *, FXSelector, void *) {
    delete this;
    return 1;
}

long FXElemBaseWindow::onPaint(FXObject *sender, FXSelector sel, void *ptr) {
    if (m_window) {
        FXEvent *ev=(FXEvent*)ptr;
        m_window->draw(ev);
    }
    // TODO: should we do some blank drawing if no window is associated ?
    return 1;
}

long FXElemBaseWindow::onMap(FXObject *, FXSelector, void *) {
    if (m_window) {
        m_window->set_thread_id();
        m_window->set_status(graphics::window_running);
    }
    return 1;
}

void FXElemBuildWindow(FXComposite *p, FXElemStartMessage *message, FXElemCreatePolicy create_flag) {
    auto elem_base_window = new FXElemBaseWindow(p, message->window, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    elem_base_window->setWidth(message->width);
    elem_base_window->setHeight(message->height);
    message->window->bind_elem_window(elem_base_window);
    if (create_flag == ELEM_CREATE_NOW) {
        elem_base_window->create();
        elem_base_window->show();
    }
}

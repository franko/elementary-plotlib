#include "fox/PlotCanvas.h"
#include "fox/window_fox.h"
#include "debug_log.h"

#include "rendering_buffer_utils.h"

FXDEFMAP(PlotCanvas) PlotCanvasMap[] = {
  FXMAPFUNC(SEL_PAINT,   PlotCanvas::ID_CANVAS,        PlotCanvas::onPaint),
  FXMAPFUNC(SEL_IO_READ, PlotCanvas::ID_UPDATE_REGION, PlotCanvas::onUpdateRegion),
};

FXIMPLEMENT(PlotCanvas,FXCanvas,PlotCanvasMap,ARRAYNUMBER(PlotCanvasMap))

long PlotCanvas::onUpdateRegion(FXObject *, FXSelector, void *ptr) {
    m_window->call_update_region();
    return 1;
}

long PlotCanvas::onPaint(FXObject *, FXSelector, void* ptr) {
    FXEvent *ev=(FXEvent*)ptr;
    debug_log("paint event");
    m_window->draw(ev);
    return 1;
}

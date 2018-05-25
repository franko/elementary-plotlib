#include "fox/PlotCanvas.h"

#include "rendering_buffer_utils.h"

FXDEFMAP(PlotCanvas) PlotCanvasMap[] = {
  //________Message_Type_____________________ID____________Message_Handler_______
  // FXMAPFUNC(SEL_PAINT,             PlotCanvas::ID_CANVAS, PlotCanvas::onPaint),
};

// Macro for the ScribbleApp class hierarchy implementation
FXIMPLEMENT(PlotCanvas,FXMainWindow,PlotCanvasMap,ARRAYNUMBER(PlotCanvasMap))

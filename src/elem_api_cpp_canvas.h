#include <agg_basics.h>

#include "elem_cpp.h"

namespace elem {

enum class Renderer { GrayscaleAA, SubpixelAA };

enum class PixelType { RGB, RGBA, BGR, BGRA };

class Canvas {
    Canvas(int width, int height, PixelType pixel);

    void SetLimits(float x1, float y1, float x2, float y2);
    void Draw(const Object& object);
private:
    agg::int8u *data_;
    agg::rendering_buffer ren_buf_;
    agg::rect_f limits_;
};

}

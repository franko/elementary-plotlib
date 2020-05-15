#include <new>

    // canvas_gen<renderer_gray_aa<pixel_type>, pixel_type>

namespace elem {

static int PixelSize(PixelType pixel) {
    return ((pixel == PixelType::RGB || pixel PixelType::BGR) ? 3 : 4);
}

Canvas::Canvas(int width, int height, PixelType pixel) :
    data_{nullptr}, ren_buf_{}, limits_{0.0, 0.0, 100.0, 100.0}
{
    if (width <= 0 || height <= 0) {
        return;
    }
    const int pixel_size = PixelSize(pixel);
    data_ = new (std::no_throw) agg::int8u[width * height * pixel_size];
    if (data_) {
        ren_buf_.attach(data_, width, height, width * pixel_size);
    }
}

void Canvas::SetLimits(float x1, float y1, float x2, float y2) {
    limits_ = agg::rect_f{x1, y1, x2, y2};
}

void Canvas::Draw(const Object& object) {
    // FIXME: figure out how to extract object_impl from object as an
    // elem_object *.
    const double sx = (width_ / (limits_.x2 - limits_.x1));
    const double sy = (height_ / (limits_.y2 - limits_.y1));
    const agg::trans_affine m{sx, 0.0, 0.0, sy, -sx * limits_.x1, -sy * limits_.y1};
    object_impl->apply_transform(m, 1.0);
    if ()
}

}

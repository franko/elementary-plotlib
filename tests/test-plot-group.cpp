#include <cmath>

#include "elem/elem.h"

using namespace elem;

// Build a reusable arrow pointing along +x, one unit long, in local coordinates.
static Group ArrowSymbol(Color color) {
    Group arrow;
    arrow.AddStroke(Path{{0, 0}, {1, 0}}, color, 1.5);
    arrow.Add(Polygon{{1, 0}, {0.7, 0.18}, {0.7, -0.18}}, color, 1, color);
    return arrow;
}

ELEM_USER_MAIN() {
    InitializeFonts();

    Plot plot;
    plot.SetTitle("Transformable groups");
    plot.SetXAxisTitle("x variable");

    Path line = FxLine(0.0001, 4 * math::Tau(), [](double x) { return std::sin(x) / x; });
    plot.AddStroke(line, color::Blue, 1.5);

    // An assembly of arrows, built once in local coordinates and reused. The
    // subgroup placement applies to the inserted copy, not to the builder.
    Group cross;
    Group arrow = ArrowSymbol(color::Red);
    for (int quarter = 0; quarter < 4; quarter++) {
        cross.AddGroup(arrow, Affine::Rotation(quarter * math::Tau() / 4));
    }

    // Data-space placement: the local geometry becomes data geometry and
    // follows the axis scaling, while stroke widths stay screen-sized.
    plot.AddGroup(cross, Affine::Scale(2.0, 0.25).Then(Affine::Translation(6, 0)));
    plot.AddGroup(cross, Affine::Scale(1.0, 0.12).Then(Affine::Translation(18, 0)));

    // Annotation placement: the anchor is in data coordinates, but the layout
    // stays in logical pixels, so resizing the window does not rescale it.
    Group label = ArrowSymbol(color::Black);
    Text caption{"first maximum", 11};
    caption.SetPosition(1.2, -0.25);
    label.Add(caption, color::None, 0, color::Black);
    plot.AddAnnotation(label, Point{0.0001, 1.0}, Affine::Scale(40, 40).Then(Affine::Translation(6, -6)));

    // Editing the builder afterwards leaves the inserted snapshots unchanged.
    label.AddStroke(Path{{0, 0}, {0, 100}}, color::Green, 1);

    Window window;
    window.Attach(plot, "");
    window.Start(640, 480, WindowResize);
    window.Wait();
    return 0;
}

ELEM_GUI_LOOP()

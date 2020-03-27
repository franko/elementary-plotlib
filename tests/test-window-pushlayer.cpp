#include "elem/elem.h"
#include "elem/elem_utils.h"

using namespace elem;

int main() {
    InitializeFonts();

    Plot p(Plot::ShowUnits);
    p.SetLimits(-1.0, 0.0, 1.0, 10.0);

    Polygon line{{-0.5, 0.0}, {-0.5, 8.0}, {0.5, 4.0}};
    p.Add(line, color::Red, 2.5, color::Yellow, property::Fill | property::Stroke);

    Window win("h.");
    win.Attach(p, "1");
    win.Start(640, 480, WindowResize);

    utils::Sleep(2);
    p.PushLayer();

    Polygon line2{{0.8, 1.0}, {0.8, 7.0}, {0.3, 4.0}};
    p.Add(line2, color::Blue, 2.5, color::None);

    utils::Sleep(2);
    p.PopLayer();

    win.Wait();
    return 0;
}

#include "elem/elem_utils.h"
#include "elem/elem.h"

using namespace elem;

int main() {
    Triangles triangles(color::Red);
    triangles.ResizePointsBuffer(6);
    triangles.ResizeTrianglesBuffer(3);
    for (int i = 0; i < 6; i++) {
        float x = (i % 2 == 0 ? 0.0f : 1.0f), y = i / 2;
        triangles.SetPoint(i, {x, y});
    }
    triangles.SetTriangle(0, {0, 1, 3});
    triangles.SetTriangle(1, {0, 3, 2});
    triangles.SetTriangle(2, {2, 5, 4});

    Plot plot;
    plot.AddElement(triangles);
    plot.SetXAxisTitle("x variable");
    plot.SetYAxisTitle("y variable");
    plot.Show(640, 480, WindowResize);
    plot.Wait();
}


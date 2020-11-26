#include "elem/elem_utils.h"
#include "elem/elem.h"

using namespace elem;

int main() {
    Plot plot;
    TestingTriangles triangles;
    plot.Add(triangles);
    plot.SetXAxisTitle("x variable");
    plot.SetYAxisTitle("y variable");
    plot.Show(640, 480, WindowResize);
    plot.Wait();
}


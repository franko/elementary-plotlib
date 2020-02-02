#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"
#include "libelplot.h"

using namespace elp;

int main() {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::bit32);
    auto elem = lua.create_table();
    elem.new_usertype<Object>("Object", sol::constructors<Object()>());
    elem.new_usertype<Path>("Path", sol::constructors<Path()>(),
        "MoveTo", &Path::MoveTo, 
        "LineTo", &Path::LineTo,
        "ClosePolygon", &Path::ClosePolygon,
        sol::base_classes, sol::bases<Object>()
    );
    elem.new_usertype<Plot>("Plot", sol::constructors<Plot(), Plot(unsigned)>(),
        "SetTitle", &Plot::SetTitle,
        "SetXAxisTitle", &Plot::SetXAxisTitle,
        "SetYAxisTitle", &Plot::SetYAxisTitle,
        "SetClipMode", &Plot::SetClipMode,
        "SetLimits", &Plot::SetLimits,
        "SetAxisLabelsAngle", &Plot::SetAxisLabelsAngle,
        "EnableLabelFormat", &Plot::EnableLabelFormat,
        "CommitPendingDraw", &Plot::CommitPendingDraw,
        "Add", &Plot::Add,
        "AddStroke", &Plot::AddStroke,
        "PushLayer", &Plot::PushLayer,
        "PopLayer", &Plot::PopLayer,
        "ClearLayer", &Plot::ClearLayer,
        "AddLegend", &Plot::AddLegend,
        "WriteSvg", &Plot::WriteSvg
    );
    elem.new_usertype<Window>("Window", sol::constructors<Window()>(),
        "Attach", &Window::Attach,
        "SetLayout", &Window::SetLayout,
        "Start", &Window::Start,
        "Wait", &Window::Wait
    );

    // Not yet done:
    // enum { ShowUnits = 1 << 0, AutoLimits = 1 << 1, ClipRegion = 1 << 2 };

    elem["xAxis"] = xAxis;
    elem["yAxis"] = yAxis;
    elem["WindowResize"] = WindowResize;

    auto elem_property = lua.create_table();
    elem_property["Fill"]    = property::Fill;
    elem_property["Stroke"]  = property::Stroke;
    elem_property["Outline"] = property::Outline;
    elem_property["Crisp"]   = property::Crisp;

    elem["property"] = elem_property;
    lua["elem"] = elem;

    InitializeFonts();

    lua.script_file("lua/sol-test.lua");
    lua.script_file("lua/sol-test-function.lua");
}

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"
#include "libelplot.h"

using namespace elp;

int main() {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::math);
    lua.new_usertype<Object>("Object", sol::constructors<Object()>());
    lua.new_usertype<Path>("Path", sol::constructors<Path()>(), 
        "MoveTo", &Path::MoveTo, 
        "LineTo", &Path::LineTo,
        "ClosePolygon", &Path::ClosePolygon,
        sol::base_classes, sol::bases<Object>()
    );
    lua.new_usertype<Plot>("Plot", sol::constructors<Plot(), Plot(unsigned)>(),
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
    lua.new_usertype<Window>("Window", sol::constructors<Window()>(),
        "Attach", &Window::Attach,
        "SetLayout", &Window::SetLayout,
        "Start", &Window::Start,
        "Wait", &Window::Wait
    );

    InitializeFonts();

    lua.script(R"(
        local plot = Plot.new()

        local line = Path.new()
        line:MoveTo(-0.5, 0)
        line:LineTo(-0.5, 8.0)
        line:LineTo(0.5, 4.0)
        line:ClosePolygon()
        plot:Add(line, 0xB40000FF, 2.5, 0xF5FE00FF, 3)

        local window = Window.new()
        window:Attach(plot, "")

        window:Start(640, 480, 1)
        window:Wait()
    )");
}

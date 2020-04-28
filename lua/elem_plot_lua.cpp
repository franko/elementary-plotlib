#include <sol/sol.hpp>

#include "elem/elem.h"
#include "elem/elem_utils.h"
#include "elem/elem_lua_cpp.h"

namespace elem {

static unsigned BitOr(sol::variadic_args va) {
    unsigned r = 0;
    for (auto v : va) {
        r = r | v.as<int>();
    }
    return r;
}

void LuaOpenLibrary(lua_State *L) {
    sol::state_view lua(L);
    auto elem = lua.create_table();
    elem.new_usertype<Object>("Object", sol::constructors<Object()>());
    elem.new_usertype<Path>("Path", sol::constructors<Path()>(),
        "MoveTo", &Path::MoveTo, 
        "LineTo", &Path::LineTo,
        "ClosePolygon", &Path::ClosePolygon,
        sol::base_classes, sol::bases<Object>()
    );
    elem.new_usertype<CurvePath>("CurvePath", sol::constructors<CurvePath()>(),
        "MoveTo", &CurvePath::MoveTo,
        "LineTo", &CurvePath::LineTo,
        "Curve3", &CurvePath::Curve3,
        "Curve4", &CurvePath::Curve4,
        "ArcTo", &CurvePath::ArcTo,
        "ClosePolygon", &CurvePath::ClosePolygon,
        sol::base_classes, sol::bases<Object>()
    );
    elem.new_usertype<DashPath>("DashPath", sol::constructors<DashPath()>(),
        "AddDash", &DashPath::AddDash,
        sol::base_classes, sol::bases<Path, Object>()
    );
    elem.new_usertype<Markers>("Markers", sol::constructors<Markers(double, Object)>(),
        sol::base_classes, sol::bases<Path, Object>()
    );
    elem.new_usertype<Text>("Text", sol::constructors<Text(const char*, double, double, double)>(),
        "SetAngle", &Text::SetAngle,
        "SetPosition", &Text::SetPosition,
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
        "Show", &Plot::Show,
        "Wait", &Plot::Wait,
        "WriteSvg", &Plot::WriteSvg
    );
    elem.new_usertype<Window>("Window", sol::constructors<Window()>(),
        "Attach", &Window::Attach,
        "SetLayout", &Window::SetLayout,
        "Start", &Window::Start,
        "Wait", &Window::Wait
    );

    elem["MarkerSymbol"] = MarkerSymbol;

    auto plot_flags = lua.create_table();
    plot_flags["ShowUnits"] = Plot::ShowUnits;
    plot_flags["AutoLimits"] = Plot::AutoLimits;
    plot_flags["ClipRegion"] = Plot::ClipRegion;
    elem["plot"] = plot_flags;

    elem["xAxis"] = xAxis;
    elem["yAxis"] = yAxis;
    elem["WindowResize"] = WindowResize;

    auto elem_property = lua.create_table();
    elem_property["Fill"]    = property::Fill;
    elem_property["Stroke"]  = property::Stroke;
    elem_property["Outline"] = property::Outline;
    elem_property["Crisp"]   = property::Crisp;

    auto elem_utils = lua.create_table();
    elem_utils["Sleep"] = utils::Sleep;
    elem["utils"] = elem_utils;

    elem["flags"] = BitOr;

    elem["property"] = elem_property;

    // TODO: problem below is that we access "elem" as a global
    // variable but there is no guarantee it is defined.
    auto fxresult = lua.script(R"(
        local function fxlinedraw(line, x0, x1, f, N)
            N = N or 128
            local dx = (x1 - x0) / N
            line:MoveTo(x0, f(x0))
            for i = 1, N do
                local x = x0 + dx * i
                line:LineTo(x, f(x))
            end
            return line
        end
        local function fxline(x0, x1, f, N)
            return fxlinedraw(elem.Path.new(), x0, x1, f, N)
        end
        return fxline, fxlinedraw
    )");
    elem["FxLine"] = fxresult.get<sol::function>(0);
    elem["FxLineDraw"] = fxresult.get<sol::function>(1);

    lua["elem"] = elem;
}
}

#include <sol/sol.hpp>

#include "libelplot.h"
#include "libelplot_utils.h"
#include "libelplot_lua_cpp.h"

namespace elp {

static unsigned BitOr(sol::variadic_args va) {
    unsigned r = 0;
    for (auto v : va) {
        r = r | v.as<int>();
    }
    return r;
}

void LuaOpenLibrary(lua_State *L) {
    sol::state_view lua(L);
    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::bit32);
    auto elp = lua.create_table();
    elp.new_usertype<Object>("Object", sol::constructors<Object()>());
    elp.new_usertype<Path>("Path", sol::constructors<Path()>(),
        "MoveTo", &Path::MoveTo, 
        "LineTo", &Path::LineTo,
        "ClosePolygon", &Path::ClosePolygon,
        sol::base_classes, sol::bases<Object>()
    );
    elp.new_usertype<CurvePath>("CurvePath", sol::constructors<CurvePath()>(),
        "MoveTo", &CurvePath::MoveTo,
        "LineTo", &CurvePath::LineTo,
        "Curve3", &CurvePath::Curve3,
        "Curve4", &CurvePath::Curve4,
        "ArcTo", &CurvePath::ArcTo,
        "ClosePolygon", &CurvePath::ClosePolygon,
        sol::base_classes, sol::bases<Object>()
    );
    elp.new_usertype<DashPath>("DashPath", sol::constructors<DashPath()>(),
        "AddDash", &DashPath::AddDash,
        sol::base_classes, sol::bases<Path, Object>()
    );
    elp.new_usertype<Markers>("Markers", sol::constructors<Markers(double, Object)>(),
        sol::base_classes, sol::bases<Path, Object>()
    );
    elp.new_usertype<Text>("Text", sol::constructors<Text(const char*, double, double, double)>(),
        "SetAngle", &Text::SetAngle,
        "SetPosition", &Text::SetPosition,
        sol::base_classes, sol::bases<Object>()
    );
    elp.new_usertype<Plot>("Plot", sol::constructors<Plot(), Plot(unsigned)>(),
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
    elp.new_usertype<Window>("Window", sol::constructors<Window()>(),
        "Attach", &Window::Attach,
        "SetLayout", &Window::SetLayout,
        "Start", &Window::Start,
        "Wait", &Window::Wait
    );

    auto plot_flags = lua.create_table();
    plot_flags["ShowUnits"] = Plot::ShowUnits;
    plot_flags["AutoLimits"] = Plot::AutoLimits;
    plot_flags["ClipRegion"] = Plot::ClipRegion;
    elp["plot"] = plot_flags;

    elp["xAxis"] = xAxis;
    elp["yAxis"] = yAxis;
    elp["WindowResize"] = WindowResize;

    auto elp_property = lua.create_table();
    elp_property["Fill"]    = property::Fill;
    elp_property["Stroke"]  = property::Stroke;
    elp_property["Outline"] = property::Outline;
    elp_property["Crisp"]   = property::Crisp;

    auto elp_utils = lua.create_table();
    elp_utils["Sleep"] = utils::Sleep;
    elp["utils"] = elp_utils;

    elp["flags"] = BitOr;

    elp["property"] = elp_property;
    lua["elp"] = elp;
}
}

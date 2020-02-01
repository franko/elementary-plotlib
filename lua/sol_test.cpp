#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"
#include "libelplot.h"

using namespace elp;

int main() {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::math);
    lua.new_usertype<Path>("Path", sol::constructors<Path()>(), 
        "MoveTo", &Path::MoveTo, 
        "LineTo", &Path::LineTo,
        "ClosePolygon", &Path::ClosePolygon
    );
    lua.script(R"(
    	local p = Path.new()
    	p:MoveTo(0, 0)
    	p:LineTo(100, 0)
    	p:LineTo(0.5, math.sqrt(3) / 2)
    	p:ClosePolygon()
    )");
}

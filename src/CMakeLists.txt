
add_library(libcanvas
    str.c agg_font_freetype.cpp render_config.cpp fatal.cpp utils.cpp
    canvas_svg.cpp markers.cpp text.cpp window_part.cpp draw_svg.cpp
    plot_axis.cpp plot.cpp units.cpp window_surface.cpp printf_check.cpp)

target_link_libraries(libcanvas ${AGG_LDFLAGS} ${FREETYPE_LDFLAGS} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(libcanvas PUBLIC ${PROJECT_SOURCE_DIR}/include ${AGG_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})

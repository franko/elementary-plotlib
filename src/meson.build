agg_dep = dependency('libagg')
freetype_dep = dependency('freetype2')
thread_dep = dependency('threads')

libcanvas_src = ['str.c', 'agg_font_freetype.cpp', 'render_config.cpp', 'fatal.cpp', 'utils.cpp', 'canvas_svg.cpp', 'markers.cpp', 'text.cpp', 'window_part.cpp', 'draw_svg.cpp', 'plot_axis.cpp', 'plot.cpp', 'units.cpp', 'window_surface.cpp', 'printf_check.cpp']

libcanvas = static_library('libcanvas', libcanvas_src, include_directories: libcanvas_include, dependencies: [agg_dep, freetype_dep, thread_dep])
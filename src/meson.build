agg_dep = dependency('libagg')
freetype_dep = dependency('freetype2')
thread_dep = dependency('threads')

libcanvas_src = ['str.c', 'agg_font_freetype.cpp', 'render_config.cpp', 'fatal.cpp', 'utils.cpp', 'canvas_svg.cpp', 'markers.cpp', 'text.cpp', 'window_part.cpp', 'draw_svg.cpp', 'plot_axis.cpp', 'plot.cpp', 'units.cpp', 'window_surface.cpp', 'printf_check.cpp', 'plot_agent.cpp']
libcanvas_cpp_api_src = ['libcanvas.cpp']
libcanvas_c_api_src = ['libcanvas_c.cpp']

libcanvaswin_cpp_api_src = files('libcanvas_window.cpp')
libcanvaswin_c_api_src = files('libcanvas_window_c.cpp')

libcanvas = static_library('canvas',
    libcanvas_src + libcanvas_cpp_api_src,
    c_args: libcanvas_cflags,
    cpp_args: libcanvas_cflags,
    include_directories: libcanvas_include,
    dependencies: [agg_dep, freetype_dep, thread_dep],
    install: true
)

libcanvas_c = static_library('canvasc',
    libcanvas_src + libcanvas_c_api_src,
    c_args: libcanvas_cflags,
    cpp_args: libcanvas_cflags,
    include_directories: libcanvas_include,
    dependencies: [agg_dep, freetype_dep, thread_dep],
    install: true
)

# The following is useful in order to let pkg.generate add -lcanvas
# in the libraries list, in addition to -lcanvaswin.
# It works by declaring libcanvas as a dependency instead of using
# link_with.
libcanvas_dep = declare_dependency(
    dependencies : [agg_dep, freetype_dep, thread_dep],
    include_directories : libcanvas_include,
    link_with : libcanvas
)

libcanvas_c_dep = declare_dependency(
    dependencies : [agg_dep, freetype_dep, thread_dep],
    include_directories : libcanvas_include,
    link_with : libcanvas_c
)

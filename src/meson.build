agg_dep = dependency('libagg')
freetype_dep = dependency('freetype2')
thread_dep = dependency('threads')

libelplot_src = ['str.c', 'agg_font_freetype.cpp', 'render_config.cpp', 'fatal.cpp', 'utils.cpp', 'canvas_svg.cpp', 'markers.cpp', 'text.cpp', 'window_part.cpp', 'draw_svg.cpp', 'plot_axis.cpp', 'plot.cpp', 'units.cpp', 'window_surface.cpp', 'printf_check.cpp', 'plot_agent.cpp']
libelplot_api_src = ['libelplot_c.cpp', 'libelplot_cpp.cpp']

libelplotwin_api_src = files('libelplot_window_c.cpp', 'libelplot_window_cpp.cpp')

libelplot = library('elplot',
    libelplot_src + libelplot_api_src,
    c_args: libelplot_cflags,
    cpp_args: libelplot_cflags,
    include_directories: libelplot_include,
    dependencies: [agg_dep, freetype_dep, thread_dep],
    install: true
)

# The following is useful in order to let pkg.generate add -lcanvas
# in the libraries list, in addition to -lcanvaswin.
# It works by declaring libelplot as a dependency instead of using
# link_with.
libelplot_dep = declare_dependency(
    dependencies : [agg_dep, freetype_dep, thread_dep],
    include_directories : libelplot_include,
    link_with : libelplot
)

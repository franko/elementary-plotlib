agg_dep = dependency('libagg')
freetype_dep = dependency('freetype2')
thread_dep = dependency('threads')

lib_elem_plot_src = ['str.c', 'agg_font_freetype.cpp', 'render_config.cpp', 'fatal.cpp', 'utils.cpp', 'canvas_svg.cpp', 'markers.cpp', 'text.cpp', 'window_part.cpp', 'draw_svg.cpp', 'plot_axis.cpp', 'plot.cpp', 'units.cpp', 'window_surface.cpp', 'printf_check.cpp', 'plot_agent.cpp']
lib_elem_plot_api_src = ['elem_plot_c.cpp', 'elem_plot_cpp.cpp']

lib_elem_plot_win_api_src = files('elem_plot_window_c.cpp', 'elem_plot_window_cpp.cpp')

lib_elem_plot = library('elemplot',
    lib_elem_plot_src + lib_elem_plot_api_src,
    c_args: lib_elem_plot_cflags,
    cpp_args: lib_elem_plot_cflags,
    include_directories: lib_elem_plot_include,
    dependencies: [agg_dep, freetype_dep, thread_dep],
    install: true
)

# The following is useful in order to let pkg.generate add -lcanvas
# in the libraries list, in addition to -lcanvaswin.
# It works by declaring lib_elem_plot as a dependency instead of using
# link_with.
lib_elem_plot_dep = declare_dependency(
    dependencies : [agg_dep, freetype_dep, thread_dep],
    include_directories : lib_elem_plot_include,
    link_with : lib_elem_plot
)

x11_dep = dependency('x11')

lib_elem_plot_xwin_src = ['x_connection.cpp', 'xwindow.cpp']

lib_elem_plot_win = library('elemplotwin', lib_elem_plot_xwin_src + lib_elem_plot_win_api_src,
    include_directories: lib_elem_plot_include,
    cpp_args: lib_elem_plot_cflags,
    dependencies: [lib_elem_plot_dep, agg_dep, freetype_dep, thread_dep, x11_dep],
    install: true
)

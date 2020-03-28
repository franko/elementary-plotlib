x11_dep = dependency('x11')

libelem_xwin_src = ['x_connection.cpp', 'xwindow.cpp']

libelem_win = library('elemwin', libelem_xwin_src + libelem_win_api_src,
    include_directories: lib_elem_plot_include,
    cpp_args: lib_elem_plot_cflags,
    dependencies: [lib_elem_plot_dep, agg_dep, freetype_dep, thread_dep, x11_dep],
    install: true
)

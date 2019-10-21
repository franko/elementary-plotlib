x11_dep = dependency('x11')

libcanvasxwin_src = ['x_connection.cpp', 'xwindow.cpp']

libcanvaswin = static_library('canvaswin', libcanvasxwin_src +  libcanvaswin_api_src,
    include_directories: libcanvas_include,
    cpp_args: libcanvas_cflags,
    dependencies: [libcanvas_dep, agg_dep, freetype_dep, thread_dep, x11_dep],
    install: true
)

x11_dep = dependency('x11')

libcanvasxwin_src = ['x_connection.cpp', 'xwindow.cpp']

libcanvasxwin = static_library('canvasxwin', libcanvasxwin_src,
    include_directories: libcanvas_include,
    cpp_args: libcanvas_cflags,
    dependencies: [agg_dep, freetype_dep, thread_dep, x11_dep],
    install: true
)

pkg.generate(libcanvasxwin,
    filebase : 'libcanvaswin',
    name : 'Libcanvaswin',
    description : 'The Window libcanvas library',
    url : 'github.com/franko/libcanvas',
)

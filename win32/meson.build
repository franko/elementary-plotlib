libcanvaswin32_src = ['agg_win32_bmp.cpp', 'window_win32.cpp']

gdi32 = cc.find_library('gdi32', required : true)

libcanvaswin32 = static_library('canvaswin32', libcanvaswin32_src + libcanvaswin_interface_src,
    include_directories: libcanvas_include,
    cpp_args: libcanvas_cflags,
    dependencies: [agg_dep, freetype_dep, gdi32, thread_dep],
    install: true
)

pkg.generate(libcanvaswin32,
    filebase : 'libcanvaswin',
    name : 'Libcanvaswin',
    description : 'The Window libcanvas library',
    url : 'github.com/franko/libcanvas',
)

libcanvaswin32_src = ['agg_win32_bmp.cpp', 'window_win32.cpp']

gdi32 = cc.find_library('gdi32', required : true)

libcanvaswin = static_library('canvaswin', libcanvaswin32_src + libcanvaswin_interface_src,
    include_directories: libcanvas_include,
    cpp_args: libcanvas_cflags,
    dependencies: [libcanvas_dep, agg_dep, freetype_dep, gdi32, thread_dep],
    install: true
)

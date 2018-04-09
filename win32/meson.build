libcanvaswin32_src = ['agg_win32_bmp.cpp', 'window_win32.cpp']

libcanvaswin32 = static_library('libcanvaswin32', libcanvaswin32_src,
    include_directories: libcanvas_include,
    dependencies: [agg_dep, freetype_dep, thread_dep])

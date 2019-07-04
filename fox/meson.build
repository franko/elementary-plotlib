fox_dep = dependency('fox')

libcanvasfoxwin_src = ['GraphicsWindow.cpp', 'window_fox.cpp']

libcanvasfoxwin = static_library('libcanvasfoxwin', libcanvasfoxwin_src,
    include_directories: libcanvas_include,
    cpp_args: libcanvas_cflags,
    dependencies: [agg_dep, freetype_dep, thread_dep, fox_dep]
)

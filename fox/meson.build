libcanvasfoxwin_src = ['GraphicsWindow.cpp', 'window_fox.cpp', 'FoxWindow.cpp']

libcanvasfox = static_library('canvasfox', libcanvasfoxwin_src,
    include_directories: libcanvas_include,
    cpp_args: libcanvas_cflags,
    dependencies: [libcanvas_dep, agg_dep, freetype_dep, thread_dep, fox_dep],
    install: true
)

pkg.generate(libcanvasfox,
    filebase : 'libcanvasfox',
    name : 'Libcanvasfox',
    description : 'The FOX Libcanvas library',
    url : 'github.com/franko/libcanvas',
)

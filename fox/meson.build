fox_dep = dependency('fox')

libcanvasfoxwin_src = ['PlotCanvas.cpp', 'window_fox.cpp']

libcanvasfoxwin = static_library('libcanvasfoxwin', libcanvasfoxwin_src,
    include_directories: libcanvas_include,
    dependencies: [agg_dep, freetype_dep, thread_dep, fox_dep])

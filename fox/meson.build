fox_dep = dependency('fox16')

libcanvasfoxwin_src = ['PlotCanvas.cpp']

libcanvasfoxwin = static_library('libcanvasfoxwin', libcanvasfoxwin_src,
    include_directories: libcanvas_include,
    dependencies: [agg_dep, freetype_dep, thread_dep, fox_dep])

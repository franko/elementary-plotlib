libelplotwin32_src = ['agg_win32_bmp.cpp', 'window_win32.cpp']

gdi32 = cc.find_library('gdi32', required : true)

libelplotwin = static_library('canvaswin', libelplotwin32_src + libelplotwin_api_src,
    include_directories: libelplot_include,
    cpp_args: libelplot_cflags,
    dependencies: [libelplot_dep, agg_dep, freetype_dep, gdi32, thread_dep],
    install: true
)

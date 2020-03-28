libelem_win32_src = ['agg_win32_bmp.cpp', 'window_win32.cpp']

gdi32 = cc.find_library('gdi32', required : true)

libelem_win = library('elemwin', libelem_win32_src + libelem_win_api_src,
    include_directories: lib_elem_plot_include,
    cpp_args: lib_elem_plot_cflags,
    dependencies: [lib_elem_plot_dep, agg_dep, freetype_dep, gdi32, thread_dep],
    install: true
)

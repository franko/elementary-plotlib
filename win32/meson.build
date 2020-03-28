libelem_win32_src = ['agg_win32_bmp.cpp', 'window_win32.cpp']

gdi32 = cc.find_library('gdi32', required : true)

libelem_win = library('elemwin', libelem_win32_src + libelem_win_api_src,
    include_directories: libelem_include,
    cpp_args: libelem_cflags,
    dependencies: [libelem_dep, agg_dep, freetype_dep, gdi32, thread_dep],
    install: true
)

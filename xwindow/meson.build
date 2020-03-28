x11_dep = dependency('x11')

libelem_xwin_src = ['x_connection.cpp', 'xwindow.cpp']

libelem_win = library('elemwin', libelem_xwin_src + libelem_win_api_src,
    include_directories: libelem_include,
    cpp_args: libelem_cflags,
    dependencies: [libelem_dep, x11_dep],
    install: true
)

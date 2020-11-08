sdl_dep = dependency('sdl2')

dl_library = cc.find_library('dl', required: false)

libelem_sdl_src = ['window_sdl.cpp', 'sdl_elem_window_factory.cpp']

libelem_win = static_library('elemwin', libelem_sdl_src + libelem_win_api_src,
    include_directories: libelem_include,
    cpp_args: libelem_cflags,
    dependencies: [libelem_dep, sdl_dep, dl_library],
    install: true
)

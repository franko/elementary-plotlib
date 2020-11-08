libelem_sdl_src = ['window_sdl.cpp', 'sdl_elem_window_factory.cpp']

libelemwin_sdl = static_library('elemwinsdl', libelem_sdl_src,
    include_directories: libelem_include,
    cpp_args: libelem_cflags,
    dependencies: [libelem_dep, sdl_dep],
    install: true
)

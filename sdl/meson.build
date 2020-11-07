libelem_sdl_src = ['window_sdl.cpp']

libelem_sdl = static_library('elemsdl', libelem_sdl_src,
    include_directories: libelem_include,
    cpp_args: libelem_cflags,
    dependencies: [libelem_dep, sdl_dep],
    install: true
)

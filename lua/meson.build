libelem_lua_src = ['elem_plot_lua.cpp', 'elem_plot_lua_c.cpp']

libelem_lua = library('elemlua',
    libelem_lua_src,
    cpp_args: libelem_cflags,
    include_directories: libelem_include,
    dependencies: [agg_dep, freetype_dep, sol_dep, lua_dep, thread_dep],
    install: true
)

pkg.generate(libelem_lua,
    filebase : 'libelemlua',
    name : 'libelemlua',
    description : 'The Elementary Plot library, Lua bindings',
    url : 'github.com/franko/elementary-plotlib',
)

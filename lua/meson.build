lib_elem_plot_lua_src = ['elem_plot_lua.cpp', 'elem_plot_lua_c.cpp']

lib_elem_plot_lua = library('elemplotlua',
    lib_elem_plot_lua_src,
    cpp_args: lib_elem_plot_cflags,
    include_directories: lib_elem_plot_include,
    dependencies: [agg_dep, freetype_dep, sol_dep, lua_dep, thread_dep],
    install: true
)

pkg.generate(lib_elem_plot_lua,
    filebase : 'libelemplotlua',
    name : 'libelemplotlua',
    description : 'The Elementary Plot library, Lua bindings',
    url : 'github.com/franko/elementary-plotlib',
)

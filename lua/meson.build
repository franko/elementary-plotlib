libelem_lua_src = ['elem_plot_lua.cpp', 'elem_plot_lua_c.cpp']

libelem_lua = library('elemlua',
    libelem_lua_src,
    cpp_args: libelem_cflags,
    include_directories: libelem_include,
    dependencies: [sol_dep, lua_dep],
    link_with: [libelem_win, libelem],
    install: true
)

pkg.generate(libelem_lua,
    filebase : 'libelemlua',
    name : 'libelemlua',
    requires: lua_dep,
    libraries: libelem_win,
    description : 'The Elementary Plot library, Lua bindings',
    url : 'github.com/franko/elementary-plotlib',
)

libelemlua_dep = declare_dependency(
    dependencies : libelemwin_dep,
    include_directories : libelem_include,
    link_with : libelem_lua
)

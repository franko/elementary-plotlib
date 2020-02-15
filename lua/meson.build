libelplotlua_src = ['libelplot_lua.cpp']

libelplotlua = library('elplotlua',
    libelplotlua_src,
    cpp_args: libelplot_cflags,
    include_directories: libelplot_include,
    dependencies: [agg_dep, freetype_dep, sol_dep, luajit_dep, thread_dep],
    install: true
)

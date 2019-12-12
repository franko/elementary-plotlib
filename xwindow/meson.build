x11_dep = dependency('x11')

libelplotxwin_src = ['x_connection.cpp', 'xwindow.cpp']

libelplotwin = library('elplotwin', libelplotxwin_src +  libelplotwin_api_src,
    include_directories: libelplot_include,
    cpp_args: libelplot_cflags,
    dependencies: [libelplot_dep, agg_dep, freetype_dep, thread_dep, x11_dep],
    install: true
)

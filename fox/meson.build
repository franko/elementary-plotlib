libelplotfoxwin_src = ['FXElemWindow.cpp', 'FXElemBaseWindow.cpp', 'window_fox.cpp']

libelplotfox = static_library('canvasfox', libelplotfoxwin_src,
    include_directories: libelplot_include,
    cpp_args: libelplot_cflags,
    dependencies: [libelplot_dep, agg_dep, freetype_dep, thread_dep, fox_dep],
    install: true
)

pkg.generate(libelplotfox,
    filebase : 'libelplotfox',
    name : 'libelplotfox',
    description : 'The FOX libelplot library',
    url : 'github.com/franko/libelplot',
)

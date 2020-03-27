lib_elem_plot_fox_src = ['FXElemWindow.cpp', 'FXElemBaseWindow.cpp', 'window_fox.cpp', 'fox_elem_window_factory.cpp', 'elem_plot_fox_cpp.cpp']

libelem_fox = static_library('elemfox', lib_elem_plot_fox_src,
    include_directories: lib_elem_plot_include,
    cpp_args: lib_elem_plot_cflags,
    dependencies: [lib_elem_plot_dep, agg_dep, freetype_dep, thread_dep, fox_dep],
    install: true
)

pkg.generate(libelem_fox,
    filebase : 'libelemfox',
    name : 'libelemfox',
    description : 'The FOX Elementary Plot library',
    url : 'github.com/franko/elementary-plotlib',
)

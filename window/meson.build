lib_elem_plot_win_api_src = files('native_elem_window_factory.cpp', 'global_elem_window_factory.cpp', 'elem_window_c.cpp', 'elem_window_cpp.cpp')

lib_elem_plot_win_api = library('elemplotwinapi', lib_elem_plot_win_api_src,
    include_directories: lib_elem_plot_include,
    cpp_args: lib_elem_plot_cflags,
    dependencies: lib_elem_plot_dep,
    install: true
)

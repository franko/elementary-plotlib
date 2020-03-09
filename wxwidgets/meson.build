executable('wx-test-dc', 'wx-test-dc.cpp',
    # include_directories: lib_elem_plot_include,
    # dependencies: [agg_dep, freetype_dep, thread_dep, fox_dep],
    dependencies: [wxwidgets_dep, thread_dep],
    # cpp_args: lib_elem_plot_cflags,
    # link_with: [lib_elem_plot_fox, lib_elem_plot_win, lib_elem_plot],
    gui_app: true
)

executable('wx-test-threads', 'wx-test-threads.cpp',
    include_directories: lib_elem_plot_include,
    # dependencies: [agg_dep, freetype_dep, thread_dep, fox_dep],
    dependencies: [wxwidgets_dep, thread_dep],
    # cpp_args: lib_elem_plot_cflags,
    link_with: lib_elem_plot,
    gui_app: true
)

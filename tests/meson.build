
window_tests = [
    {'name': 'test-window', 'sources': 'test-window.cpp'},
    {'name': 'test-window-auto', 'sources': 'test-window-auto.cpp'},
    {'name': 'test-window-set-layout', 'sources': 'test-window-set-layout.cpp'},
    {'name': 'test-window-pushlayer', 'sources': 'test-window-pushlayer.cpp'},
    {'name': 'test-window-function', 'sources': 'test-window-function.cpp'},
    {'name': 'test-window-markers', 'sources': 'test-window-markers.cpp'},
    {'name': 'test-window-anim', 'sources': 'test-window-anim.cpp'},
    {'name': 'test-custom-render-pipeline', 'sources': 'test-custom-render-pipeline.cpp'},
    {'name': 'test-window-outline', 'sources': 'test-window-outline.cpp'},
    {'name': 'test-plot-legend', 'sources': 'test-plot-legend.cpp'},
    {'name': 'test-plot-copy', 'sources': 'test-plot-copy.cpp'},
    {'name': 'test-plot-dash-lines', 'sources': 'test-plot-dash-lines.cpp'},
    {'name': 'test-bezier', 'sources': 'test-bezier.cpp'},
    {'name': 'test-write-svg', 'sources': 'test-write-svg.cpp'},
    {'name': 'test-plot-histogram', 'sources': 'test-plot-histogram.cpp'},
    {'name': 'test-window-c', 'sources': 'test-window-c.c'},
    {'name': 'test-bezier-c', 'sources': 'test-bezier-c.c'},
    {'name': 'test-plot-dash-lines-c', 'sources': 'test-plot-dash-lines-c.c'},
    {'name': 'test-window-markers-c', 'sources': 'test-window-markers-c.c'},
]

fox_tests = [
    {'name': 'test-fox-window', 'sources': 'test-fox-window.cpp'},
    {'name': 'test-fox-window-2', 'sources': 'test-fox-window-2.cpp'},
    {'name': 'test-fox-window-3', 'sources': 'test-fox-window-3.cpp'},
    {'name': 'test-fox-host', 'sources': 'test-fox-window-host-system.cpp'},
]

foreach test : window_tests
    executable(test['name'], test['sources'],
        include_directories: libelplot_include,
        dependencies: libelplot_dep,
        cpp_args: libelplot_cflags,
        link_with: [libelplotwin, libelplot],
        gui_app: false)
endforeach

if fox_dep.found()
    foreach test : fox_tests
        executable(test['name'], test['sources'],
            include_directories: libelplot_include,
            dependencies: [agg_dep, freetype_dep, thread_dep, fox_dep],
            cpp_args: libelplot_cflags,
            link_with: [libelplotfox, libelplot],
            gui_app: false)
    endforeach
endif

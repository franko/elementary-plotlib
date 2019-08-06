if host_machine.system() == 'linux'
    libcanvaswin = libcanvasxwin
else
    libcanvaswin = libcanvaswin32
endif

window_tests = [
    {'name': 'test-window', 'sources': 'test-window.cpp'},
    {'name': 'test-window-auto', 'sources': 'test-window-auto.cpp'},
    {'name': 'test-window-pushlayer', 'sources': 'test-window-pushlayer.cpp'},
    {'name': 'test-window-function', 'sources': 'test-window-function.cpp'},
    {'name': 'test-window-markers', 'sources': 'test-window-markers.cpp'},
    {'name': 'test-window-anim', 'sources': 'test-window-anim.cpp'},
    {'name': 'test-custom-render-pipeline', 'sources': 'test-custom-render-pipeline.cpp'},
    {'name': 'test-window-outline', 'sources': 'test-window-outline.cpp'},
    {'name': 'test-pointer-container', 'sources': 'test-pointer-container.cpp'},
    {'name': 'test-plot-legend', 'sources': 'test-plot-legend.cpp'},
    {'name': 'test-plot-copy', 'sources': 'test-plot-copy.cpp'},
    {'name': 'test-bezier', 'sources': 'test-bezier.cpp'},
]

fox_tests = [
    {'name': 'test-fox-window', 'sources': 'test-fox-window.cpp'},
    {'name': 'test-fox-window-2', 'sources': 'test-fox-window-2.cpp'},
    {'name': 'test-fox-window-3', 'sources': 'test-fox-window-3.cpp'},
]

foreach test : window_tests
    executable(test['name'], test['sources'],
        include_directories: libcanvas_include,
        dependencies: [agg_dep, freetype_dep, thread_dep],
        cpp_args: libcanvas_cflags,
        link_with: [libcanvas, libcanvaswin],
        gui_app: false)
endforeach

if fox_dep.found()
    foreach test : fox_tests
        executable(test['name'], test['sources'],
            include_directories: libcanvas_include,
            dependencies: [agg_dep, freetype_dep, thread_dep, fox_dep],
            cpp_args: libcanvas_cflags,
            link_with: [libcanvas, libcanvasfoxwin],
            gui_app: false)
    endforeach
endif

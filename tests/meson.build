
window_tests = [
    {'name': 'test-window', 'sources': 'test-window.cpp'},
    {'name': 'test-window-gc', 'sources': 'test-window-gc.cpp'},
    {'name': 'test-window-gc-1', 'sources': 'test-window-gc-1.cpp'},
    {'name': 'test-window-close', 'sources': 'test-window-close.cpp'},
    {'name': 'test-window-no-init', 'sources': 'test-window-no-init.cpp'},
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
    {'name': 'test-plot-show', 'sources': 'test-plot-show.cpp'},
    {'name': 'test-plot-show-2', 'sources': 'test-plot-show-2.cpp'},
    {'name': 'test-plot-function-c', 'sources': 'test-plot-function-c.c'},
    {'name': 'test-window-c', 'sources': 'test-window-c.c'},
    {'name': 'test-bezier-c', 'sources': 'test-bezier-c.c'},
    {'name': 'test-plot-dash-lines-c', 'sources': 'test-plot-dash-lines-c.c'},
    {'name': 'test-plot-show-c', 'sources': 'test-plot-show-c.c'},
    {'name': 'test-window-markers-c', 'sources': 'test-window-markers-c.c'},
]

fox_tests = [
    {'name': 'test-fox-window', 'sources': 'test-fox-window.cpp'},
    {'name': 'test-fox-window-2', 'sources': 'test-fox-window-2.cpp'},
    {'name': 'test-fox-window-3', 'sources': 'test-fox-window-3.cpp'},
    {'name': 'test-fox-host', 'sources': 'test-fox-host.cpp'},
    {'name': 'test-fox-host-2', 'sources': 'test-fox-host-2.cpp'},
    {'name': 'test-fox-host-3', 'sources': 'test-fox-host-3.cpp'},
    {'name': 'test-fox-host-3b', 'sources': 'test-fox-host-3b.cpp'},
    {'name': 'test-fox-host-3c', 'sources': 'test-fox-host-3c.cpp'},
    {'name': 'test-fox-host-3d', 'sources': 'test-fox-host-3d.cpp'},
    {'name': 'test-fox-host-3e', 'sources': 'test-fox-host-3e.cpp'},
    {'name': 'test-fox-host-close', 'sources': 'test-fox-host-close.cpp'},
]

foreach test : window_tests
    executable(test['name'], test['sources'],
        include_directories: libelem_include,
        dependencies: libelem_dep,
        cpp_args: libelem_cflags,
        link_with: [libelem_win, libelem],
        gui_app: false)
endforeach

if enable_fox
    foreach test : fox_tests
        executable(test['name'], test['sources'],
            include_directories: libelem_include,
            dependencies: [agg_dep, freetype_dep, thread_dep, fox_dep],
            cpp_args: libelem_cflags,
            link_with: [libelem_fox, libelem_win, libelem],
            gui_app: false)
    endforeach
endif

if enable_lua
    executable('lua-run', 'lua-run.cpp',
        include_directories: libelem_include,
        dependencies: [libelem_dep, sol_dep, lua_dep],
        cpp_args: libelem_cflags,
        link_with: [libelem_win, libelem_lua, libelem],
        gui_app: false
    )

    executable('lua-run-c', 'lua-run-c.c',
        include_directories: libelem_include,
        dependencies: [libelem_dep, sol_dep, lua_dep],
        cpp_args: libelem_cflags,
        link_with: [libelem_win, libelem_lua, libelem],
        gui_app: false
    )
endif

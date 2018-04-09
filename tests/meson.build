if host_machine.system() == 'linux'
    test_xwindow_src = ['test-xwindow.cpp']

    executable('test-xwindow', test_xwindow_src,
               include_directories: [libcanvas_include],
               dependencies: [agg_dep, freetype_dep, thread_dep, x11_dep],
               link_with: [libcanvas, libcanvasxwin],
               gui_app: false)
else
    test_win32_src = ['test-win32.cpp']

    executable('test-win32', test_win32_src,
               include_directories: [libcanvas_include],
               dependencies: [agg_dep, freetype_dep, thread_dep],
               link_with: [libcanvas, libcanvaswin32],
               gui_app: false)
endif

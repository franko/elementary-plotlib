if host_machine.system() == 'linux'
    executable('test-xwindow', ['test-xwindow.cpp'],
               include_directories: [libcanvas_include],
               dependencies: [agg_dep, freetype_dep, thread_dep, x11_dep],
               link_with: [libcanvas, libcanvasxwin],
               gui_app: false)
else
    executable('test-win32', ['test-win32.cpp'],
               include_directories: [libcanvas_include],
               dependencies: [agg_dep, freetype_dep, thread_dep],
               link_with: [libcanvas, libcanvaswin32],
               gui_app: false)

    executable('test-auto-win32', ['test-auto-win32.cpp'],
               include_directories: [libcanvas_include],
               dependencies: [agg_dep, freetype_dep, thread_dep],
               link_with: [libcanvas, libcanvaswin32],
               gui_app: false)

    executable('test-win32-pushlayer', ['test-win32-pushlayer.cpp'],
               include_directories: [libcanvas_include],
               dependencies: [agg_dep, freetype_dep, thread_dep],
               link_with: [libcanvas, libcanvaswin32],
               gui_app: false)
endif

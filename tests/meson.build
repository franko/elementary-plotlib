x11_dep = dependency('libx11')

test_xwindow_src = ['test-xwindow.cpp']

executable('test-xwindow', test_xwindow_src,
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep, x11_dep],
           link_with: [libcanvas, libcanvasxwin],
           gui_app: false)

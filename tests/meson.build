if host_machine.system() == 'linux'
  libcanvaswin = libcanvasxwin
else
  libcanvaswin = libcanvaswin32
endif

executable('test-window', ['test-window.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep],
           link_with: [libcanvas, libcanvaswin],
           gui_app: false)

executable('test-window-auto', ['test-window-auto.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep],
           link_with: [libcanvas, libcanvaswin],
           gui_app: false)

executable('test-window-pushlayer', ['test-window-pushlayer.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep],
           link_with: [libcanvas, libcanvaswin],
           gui_app: false)

executable('test-window-function', ['test-window-function.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep],
           link_with: [libcanvas, libcanvaswin],
           gui_app: false)

executable('test-window-markers', ['test-window-markers.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep],
           link_with: [libcanvas, libcanvaswin],
           gui_app: false)

executable('test-window-anim', ['test-window-anim.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep],
           link_with: [libcanvas, libcanvaswin],
           gui_app: false)

executable('test-custom-render-pipeline', ['test-custom-render-pipeline.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep],
           link_with: [libcanvas, libcanvaswin],
           gui_app: false)

executable('test-window-outline', ['test-window-outline.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep],
           link_with: [libcanvas, libcanvaswin],
           gui_app: false)

executable('test-pointer-container', ['test-pointer-container.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep],
           link_with: [libcanvas, libcanvaswin],
           gui_app: false)

executable('test-fox-window', ['test-fox-window.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep, fox_dep],
           link_with: [libcanvas, libcanvasfoxwin],
           gui_app: false)

executable('test-fox-window-2', ['test-fox-window-2.cpp'],
           include_directories: [libcanvas_include],
           dependencies: [agg_dep, freetype_dep, thread_dep, fox_dep],
           link_with: [libcanvas, libcanvasfoxwin],
           gui_app: false)


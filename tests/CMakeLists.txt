if (WIN32)
  set(LIB_CANVAS_WIN libcanvaswin32)
else (WIN32)
  set(LIB_CANVAS_WIN libcanvasxwin)
endif (WIN32)

add_executable(test-window test-window.cpp)
target_link_libraries(test-window ${LIB_CANVAS_WIN})

add_executable(test-window-function test-window-function.cpp)
target_link_libraries(test-window-function ${LIB_CANVAS_WIN})

add_executable(test-window-markers test-window-markers.cpp)
target_link_libraries(test-window-markers ${LIB_CANVAS_WIN})

add_executable(test-custom-render-pipeline test-custom-render-pipeline.cpp)
target_link_libraries(test-custom-render-pipeline ${LIB_CANVAS_WIN})

add_executable(test-window-outline test-window-outline.cpp)
target_link_libraries(test-window-outline ${LIB_CANVAS_WIN})

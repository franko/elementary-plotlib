if (WIN32)
  set(LIB_CANVAS_WIN libcanvaswin32)
else (WIN32)
  set(LIB_CANVAS_WIN libcanvasxwin)
endif (WIN32)

add_executable(test-window test-window.cpp)
target_link_libraries(test-window ${LIB_CANVAS_WIN})

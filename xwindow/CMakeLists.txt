pkg_search_module(X11 REQUIRED x11)

add_library(libcanvasxwin x_connection.cpp xwindow.cpp)

target_link_libraries(libcanvasxwin libcanvas ${X11_LIBRARIES})

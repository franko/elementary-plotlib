add_library(libcanvaswin32 agg_win32_bmp.cpp window_win32.cpp)

target_link_libraries(libcanvaswin32 libcanvas)

agg_dep = dependency('libagg')
freetype_dep = dependency('freetype2')
thread_dep = dependency('threads')

libelem_src = [
    'str.c',
    'agg_font_freetype.cpp',
    'render_config.cpp',
    'fatal.cpp',
    'utils.cpp',
    'canvas_svg.cpp',
    'markers.cpp',
    'text.cpp',
    'sg_drawing_element.cpp',
    'window_part.cpp',
    'draw_svg.cpp',
    'plot_axis.cpp',
    'plot.cpp',
    'units.cpp',
    'window_surface.cpp',
    'printf_check.cpp',
    'plot_agent.cpp',
    'elem_plot.cpp',
    'gc_context.cpp'
]
libelem_api_src = ['elem_c.cpp', 'elem_cpp.cpp']

libelem = library('elem',
    libelem_src + libelem_api_src,
    c_args: libelem_cflags,
    cpp_args: libelem_cflags,
    include_directories: libelem_include,
    dependencies: [agg_dep, freetype_dep, thread_dep],
    install: true
)

libelem_dep = declare_dependency(
    dependencies : [agg_dep, freetype_dep, thread_dep],
    include_directories : libelem_include,
    link_with : libelem
)

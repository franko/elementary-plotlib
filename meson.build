project('elementary_plotlib', 'c', 'cpp', version: '0.1.0', default_options : ['c_std=c99' ,'cpp_std=c++17'])

# ifeq ($(strip $(DISABLE_GAMMA_CORR)), yes)
#  DEFS += -DDISABLE_GAMMA_CORR
# endif

# ifeq ($(strip $(DISABLE_SUBPIXEL_LCD)), yes)
#  DEFS += -DDISABLE_SUBPIXEL_AA
# endif

lib_elem_plot_cflags = []

host_system = host_machine.system()
host_is_window = (host_system == 'windows') or (host_system == 'cygwin')

if host_system == 'windows'
    lib_elem_plot_cflags += '-DWIN32'
elif host_system == 'cygwin'
    lib_elem_plot_cflags += '-DCYGWIN'
endif

debug_build = get_option('buildtype') != 'release'
cc = meson.get_compiler('cpp')

if debug_build
    lib_elem_plot_cflags += '-DELEM_PLOT_DEBUG'
    lib_elem_plot_cflags += '-DELEM_PLOT_DEBUG_LEVEL=' + get_option('debuglevel').to_string()
endif

lib_elem_plot_include = include_directories('include')
install_headers('include/elem_plot_cpp.h')
install_headers('include/elem_plot_c.h')
install_headers('include/elem_plot.h')
install_headers('include/elem_plot_constants.h')
install_headers('include/elem_plot_generator.h')
install_headers('include/elem_plot_c_forward.h')
install_headers('include/elem_plot_lua.h')
install_headers('include/elem_plot_lua_c.h')
install_headers('include/elem_plot_lua_cpp.h')
install_headers('include/FXElemWindow.h')

subdir('src')
subdir('window')
if host_system == 'linux'
    subdir('xwindow')
elif host_is_window
    subdir('win32')
else
    error('Host not supported \'' + host_machine.system() + '\'.')
endif

pkg = import('pkgconfig')

pkg.generate(lib_elem_plot_win,
    filebase : 'libelemplot',
    name : 'libelemplot',
    description : 'The Elementary Plot library',
    url : 'github.com/franko/elementary-plotlib',
)

fox_dep = dependency('fox', required: false)

if fox_dep.found()
    subdir('fox')
endif

wxwidgets_dep = dependency('wxwidgets', required: false)

if wxwidgets_dep.found()
    subdir('wxwidgets')
endif

sol_dep = dependency('sol2', required: false)

luajit_dep  = dependency('luajit', required: false)
puc_rio_lua_dep = dependency('lua', required: false)
if luajit_dep.found() or not puc_rio_lua_dep.found()
    lua_dep = luajit_dep
else    
    lua_dep = puc_rio_lua_dep
endif

enable_lua = (sol_dep.found() and lua_dep.found())

if enable_lua
    lib_elem_plot_cflags += '-DELEM_PLOT_USE_LUA'
    subdir('lua')
endif

if get_option('tests')
    subdir('tests')
endif

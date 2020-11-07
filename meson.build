project('elementary_plotlib', 'c', 'cpp', version: '1.0-beta3', default_options : ['c_std=c99' ,'cpp_std=c++17'])

host_system = host_machine.system()
host_is_window = (host_system == 'windows') or (host_system == 'cygwin')

libelem_cflags = []
if host_system == 'windows'
    libelem_cflags += '-DWIN32'
elif host_system == 'cygwin'
    libelem_cflags += '-DCYGWIN'
endif

debug_build = get_option('buildtype') != 'release'
cc = meson.get_compiler('cpp')

if debug_build
    libelem_cflags += '-DELEM_PLOT_DEBUG'
    libelem_cflags += '-DELEM_PLOT_DEBUG_LEVEL=' + get_option('debuglevel').to_string()
endif

libelem_include = include_directories('include')
install_headers('include/elem/elem_cpp.h', subdir: 'elem')
install_headers('include/elem/elem_c.h', subdir: 'elem')
install_headers('include/elem/elem.h', subdir: 'elem')
install_headers('include/elem/elem_utils.h', subdir: 'elem')
install_headers('include/elem/elem_constants.h', subdir: 'elem')
install_headers('include/elem/elem_generator.h', subdir: 'elem')
install_headers('include/elem/elem_c_forward.h', subdir: 'elem')

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

pkg.generate(libelem_win,
    filebase : 'libelem',
    name : 'libelem',
    libraries : libelem,
    description : 'The Elementary Plot library',
    url : 'github.com/franko/elementary-plotlib',
)

fox_dep = dependency('fox', required: false)
enable_fox = (get_option('fox') and fox_dep.found())

sdl_dep = dependency('sdl2')

subdir('sdl')

if enable_fox
    subdir('fox')
    install_headers('include/elem/elem_fox.h', subdir: 'elem')
    install_headers('include/elem/elem_fox_cpp.h', subdir: 'elem')
    install_headers('include/FXElemWindow.h')
    install_headers('include/FXElemBaseWindow.h')
endif

sol_dep = dependency('sol2', required: false)

luajit_dep  = dependency('luajit', required: false)
puc_rio_lua_dep = dependency('lua', required: false)
if luajit_dep.found() or not puc_rio_lua_dep.found()
    lua_dep = luajit_dep
else    
    lua_dep = puc_rio_lua_dep
endif

enable_lua = (get_option('lua') and sol_dep.found() and lua_dep.found())

if enable_lua
    libelem_cflags += '-DELEM_PLOT_USE_LUA'
    subdir('lua')
    install_headers('include/elem/elem_lua.h', subdir: 'elem')
    install_headers('include/elem/elem_lua_c.h', subdir: 'elem')
    install_headers('include/elem/elem_lua_cpp.h', subdir: 'elem')
endif

if get_option('tests')
    subdir('tests')
endif

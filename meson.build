project('libcanvas', 'c', 'cpp', version: '0.1.0', default_options : ['c_std=c99' ,'cpp_std=c++14'])

# ifeq ($(strip $(DISABLE_GAMMA_CORR)), yes)
#  DEFS += -DDISABLE_GAMMA_CORR
# endif

# ifeq ($(strip $(DISABLE_SUBPIXEL_LCD)), yes)
#  DEFS += -DDISABLE_SUBPIXEL_AA
# endif

libcanvas_cflags = []

if host_machine.system() == 'windows'
    libcanvas_cflags += '-DWIN32'
endif

debug_build = get_option('buildtype') != 'release'
cc = meson.get_compiler('cpp')

if debug_build
    libcanvas_cflags += '-DLIBCANVAS_DEBUG'
    libcanvas_cflags += '-DLIBCANVAS_DEBUG_LEVEL=' + get_option('debuglevel').to_string()
endif

libcanvas_include = include_directories('include')
install_headers('include/libcanvas_cpp.h')
install_headers('include/libcanvas_c.h')
install_headers('include/libcanvas.h')
install_headers('include/FXLibcanvasWindow.h')

subdir('src')
if host_machine.system() == 'linux'
    subdir('xwindow')
else
    subdir('win32')
endif

pkg = import('pkgconfig')

pkg.generate(libcanvaswin,
    filebase : 'libcanvas',
    name : 'Libcanvas',
    description : 'The Libcanvas library',
    url : 'github.com/franko/libcanvas',
)

fox_dep = dependency('fox', required: false)

if fox_dep.found()
    subdir('fox')
endif

if get_option('tests')
    subdir('tests')
endif

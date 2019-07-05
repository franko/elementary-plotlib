project('libcanvas', 'c', 'cpp', version: '0.1.0', default_options : ['c_std=c99' ,'cpp_std=c++14'])

pkg = import('pkgconfig')

# ifeq ($(strip $(DISABLE_GAMMA_CORR)), yes)
#  DEFS += -DDISABLE_GAMMA_CORR
# endif

# ifeq ($(strip $(DISABLE_SUBPIXEL_LCD)), yes)
#  DEFS += -DDISABLE_SUBPIXEL_AA
# endif

libcanvas_cflags = ['-D_REENTRANT']

if host_machine.system() == 'windows'
    libcanvas_cflags += '-DWIN32'
endif

debug_build = get_option('buildtype') != 'release'

if debug_build
    libcanvas_cflags += '-DLIBCANVAS_DEBUG'
endif

libcanvas_include = include_directories('include')
install_subdir('include', install_dir: 'include/libcanvas', strip_directory: true)

subdir('src')
if host_machine.system() == 'linux'
    subdir('xwindow')
else
    subdir('win32')
endif

fox_dep = dependency('fox', required: false)

if fox_dep.found()
    subdir('fox')
endif

if get_option('tests')
    subdir('tests')
endif

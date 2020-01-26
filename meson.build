project('libelplot', 'c', 'cpp', version: '0.1.0', default_options : ['c_std=c99' ,'cpp_std=c++14'])

# ifeq ($(strip $(DISABLE_GAMMA_CORR)), yes)
#  DEFS += -DDISABLE_GAMMA_CORR
# endif

# ifeq ($(strip $(DISABLE_SUBPIXEL_LCD)), yes)
#  DEFS += -DDISABLE_SUBPIXEL_AA
# endif

libelplot_cflags = []

host_system = host_machine.system()
host_is_window = (host_system == 'windows') or (host_system == 'cygwin')

if host_system == 'windows'
    libelplot_cflags += '-DWIN32'
elif host_system == 'cygwin'
    libelplot_cflags += '-DCYGWIN'
endif

debug_build = get_option('buildtype') != 'release'
cc = meson.get_compiler('cpp')

if debug_build
    libelplot_cflags += '-Dlibelplot_DEBUG'
    libelplot_cflags += '-Dlibelplot_DEBUG_LEVEL=' + get_option('debuglevel').to_string()
endif

libelplot_include = include_directories('include')
install_headers('include/libelplot_cpp.h')
install_headers('include/libelplot_c.h')
install_headers('include/libelplot.h')
install_headers('include/libelplot_constants.h')
install_headers('include/libelplot_generator.h')
install_headers('include/libelplot_c_forward.h')
install_headers('include/FXElemWindow.h')

subdir('src')
if host_system == 'linux'
    subdir('xwindow')
elif host_is_window
    subdir('win32')
else
    error('Host not supported \'' + host_machine.system() + '\'.')
endif

pkg = import('pkgconfig')

pkg.generate(libelplotwin,
    filebase : 'libelplot',
    name : 'libelplot',
    description : 'The libelplot library',
    url : 'github.com/franko/libelplot',
)

fox_dep = dependency('fox', required: false)

if fox_dep.found()
    subdir('fox')
endif

subdir('lua')
if get_option('tests')
    subdir('tests')
endif

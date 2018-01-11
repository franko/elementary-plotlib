project('libcavas', 'c', 'cpp', version: '0.1.0', default_options : ['c_std=c99' ,'cpp_std=c++14'])

# ifeq ($(strip $(DISABLE_GAMMA_CORR)), yes)
#  DEFS += -DDISABLE_GAMMA_CORR
# endif

# ifeq ($(strip $(DISABLE_SUBPIXEL_LCD)), yes)
#  DEFS += -DDISABLE_SUBPIXEL_AA
# endif

if host_machine.system() == 'windows'
    add_global_arguments('-DWIN32', language : ['c', 'cpp'])
endif

debug_build = get_option('buildtype') != 'release'

add_global_arguments('-D_REENTRANT', language : ['c', 'cpp'])
if debug_build
    add_global_arguments('-DGSL_SHELL_DEBUG', language : ['c', 'cpp'])
endif

if meson.get_compiler('cpp').get_id() == 'gcc'
    add_global_arguments('-ffast-math', language : ['c', 'cpp'])
    if not debug_build
        add_global_arguments(['-fno-exceptions', '-fno-rtti'], language : 'cpp')
    endif
endif

libcanvas_include = include_directories('include')

subdir('src')
if host_machine.system() == 'linux'
    subdir('xwindow')
    subdir('tests')
endif

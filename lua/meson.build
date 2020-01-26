luajit_dep = dependency('luajit')
# luabridge_dep = dependency('luabridge')

bridge_test = executable('bridge-test',
    ['bridge-test.cpp'],
    dependencies: [libelplot_dep, luajit_dep],
    install: false,
    include_directories: libelplot_include,
    link_with: [libelplotwin, libelplot],
    gui_app: false
)

sol_dep = dependency('sol2')
luajit_dep  = dependency('luajit')

sol_test_1 = executable('sol-test-1',
	'sol_test.cpp',
	dependencies: [sol_dep, luajit_dep, libelplot_dep],
    link_with: [libelplotwin, libelplot],
)

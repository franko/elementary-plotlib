local ffi = require("ffi")
local elp = require("elplot")
local elw = require("elplotwin")
elp.elp_initialize_fonts()
local win = elw.elp_window_new_with_layout("v..")

local function path_of_function(f, x1, x2, n)
	local path = elp.elp_path_new()
	local x, dx = x1, (x2 - x1) / n
	while x <= x2 do
		local y = f(x)
		elp.elp_path_line_to(path, x, y)
		x = x + dx
	end
	return path
end

local plot1 = elp.elp_plot_new(bit.bor(elp.elp_plot_show_units, elp.elp_plot_auto_limits))
local path1 = path_of_function(function(x) return math.sin(x*x) end, -5, 5, 256)
elp.elp_plot_add(plot1, ffi.cast('elp_object *', path1), elp.elp_color_blue, 1.5, 0, elp.elp_stroke)
elw.elp_window_attach(win, plot1, "1")

local plot2 = elp.elp_plot_new(bit.bor(elp.elp_plot_show_units, elp.elp_plot_auto_limits))
local path2 = path_of_function(function(x) return math.cos(x*x) end, -5, 5, 256)
elp.elp_plot_add(plot2, ffi.cast('elp_object *', path2), elp.elp_color_red, 1.5, 0, elp.elp_stroke)
elw.elp_window_attach(win, plot2, "2")

elw.elp_window_start(win, 640, 480, elw.elp_window_resize)

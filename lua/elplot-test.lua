local ffi = require"ffi"
local elp = require("elplot")
local elw = require("elplotwin")
elp.elp_initialize_fonts()
local plot = elp.elp_plot_new(bit.bor(elp.elp_plot_show_units, elp.elp_plot_auto_limits))
local path = elp.elp_path_new()
for i = -128, 128 do
    local x = -5 * i / 128
    local y = math.sin(x*x)
    elp.elp_path_line_to(path, x, y)
end
elp.elp_plot_add(plot, ffi.cast('elp_object *', path), elp.elp_color_blue, 1.5, 0, elp.elp_stroke)
local win = elw.elp_window_new()
elw.elp_window_attach(win, plot, "")
elw.elp_window_start(win, 640, 480, elw.elp_window_resize)

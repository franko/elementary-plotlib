local bit = require('bit')
local defs = require("lua.bridge-defs")
local bor = bit.bor

local line = elem.Path()
line:MoveTo(-50, 0)
line:LineTo(-50, 80)
line:LineTo(50, 40)
line:ClosePolygon()

local plot = elem.Plot(bor(defs.plot.AutoLimits, defs.plot.ShowUnits))
plot:Add(line, defs.color.Red, 1.5, defs.color.Yellow, bor(defs.property.Stroke, defs.property.Fill))

local window = elem.Window(".")
-- window:Attach(plot, "")
window:Start(640, 480, defs.window.WindowResize)
window:Wait()

print('everything ok!')

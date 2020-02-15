local plot = elp.Plot.new()

local line = elp.Path.new()
line:MoveTo(-0.5, 0)
line:LineTo(-0.5, 8.0)
line:LineTo(0.5, 4.0)
line:ClosePolygon()
plot:Add(line, 0xB40000FF, 2.5, 0xF5FE00FF, bit.bor(elp.property.Stroke, elp.property.Fill))

local window = elp.Window.new()
window:Attach(plot, "")

window:Start(640, 480, elp.WindowResize)
window:Wait()

local plot = elem.Plot.new()

local line = elem.Path.new()
line:MoveTo(-0.5, 0)
line:LineTo(-0.5, 8.0)
line:LineTo(0.5, 4.0)
line:ClosePolygon()
plot:Add(line, 0xB40000FF, 2.5, 0xF5FE00FF, bit.bor(elem.property.Stroke, elem.property.Fill))

local window = elem.Window.new()
window:Attach(plot, "")

window:Start(640, 480, elem.WindowResize)
window:Wait()

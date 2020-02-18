local plot = elem.Plot.new(elem.plot.ShowUnits)
plot:SetLimits(-1.0, 0.0, 1.0, 10.0)
plot:SetAxisLabelsAngle(elem.xAxis, 3.141592 / 4)
plot:EnableLabelFormat(elem.xAxis, "%.6f")

local line = elem.Path.new()
line:MoveTo(-0.5, 0)
line:LineTo(-0.5, 8.0)
line:LineTo(0.5, 4.0)
line:ClosePolygon()
plot:Add(line, 0xB40000FF, 2.5, 0xF5FE00FF, elem.flags(elem.property.Stroke, elem.property.Fill))

local window = elem.Window.new()
window:Attach(plot, "")

window:Start(640, 480, elem.WindowResize)
elem.utils.Sleep(4)

local line2 = elem.Path.new()
line2:MoveTo(0.8, 1.0)
line2:LineTo(0.8, 7.0)
line2:LineTo(0.3, 4.0)
line2:ClosePolygon()
plot:Add(line2, 0x0000B4FF, 2.5, 0x00000000, elem.property.Stroke)

window:Wait()

local plot = elem.Plot.new()
local x0, x1 = 0.0001, 8 * math.pi * 2
plot:AddStroke(elem.FxLine(x0, x1, function(x) return math.sin(x) / x end), 0x0000B4FF, 1.5, elem.property.Stroke)
plot:SetTitle("Function plot example")
plot:SetXAxisTitle("x variable")
plot:SetYAxisTitle("y variable")
plot:Show(640, 480, elem.WindowResize)
plot:AddStroke(elem.FxLine(0.8, x1, function(x) return math.cos(x) / x end), 0xB40000FF, 1.5, elem.property.Stroke)
plot:Wait()

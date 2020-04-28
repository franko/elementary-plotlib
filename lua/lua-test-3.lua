local random, sin, cos, sqrt, log, pi = math.random, math.sin, math.cos, math.sqrt, math.log, math.pi

local function gaussian_random(mean, sigma)
    -- https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
    local u1, u2 = math.random(), math.random()
    local r = sqrt(-2 * log(u1))
    return mean + sigma * r * cos(2 * pi * u2)
end

local markers = elem.Markers.new(6.0, elem.MarkerSymbol(1))
local x0, x1 = 0.0001, 8 * 2 * pi
local points_number = 37
elem.FxLineDraw(markers, x0, x1, function(x)
    return sin(x) / x + gaussian_random(0, 0.02)
end, points_number)
local line = elem.Path.new()
elem.FxLineDraw(line, x0, x1, function(x)
    return sin(x) / x
end)

local plot = elem.Plot.new()
plot:AddStroke(line, 0xB40000FF, 1.5, elem.property.Stroke)
plot:AddStroke(markers, 0x0000B4FF, 1.5, elem.property.Stroke)
plot:SetTitle("Function plot example")
plot:SetXAxisTitle("x variable")
plot:Show(640, 480, elem.WindowResize)
plot:Wait()

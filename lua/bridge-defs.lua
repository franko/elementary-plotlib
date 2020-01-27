local color = {
    Red = 0xB40000FF, Green = 0x00CC00ff, Blue = 0x0000B4FF, Yellow = 0xF5FE00FF, Black = 0x000000FF, Gray = 0xBBBBBBFF, White = 0xFFFFFFFF, None = 0
}

local property = {
    Fill    = 1,
    Stroke  = 2,
    Outline = 4,
    Crisp   = 8,
}

local plot = { ShowUnits = 1, AutoLimits = 2, ClipRegion = 4 }

local window = {
    WindowResize          = 1,
    WindowHwBuffer        = 2,
    WindowKeepAspectRatio = 4,
    WindowProcessAllKeys  = 8,
}

return {color= color, property= property, plot= plot, window= window}

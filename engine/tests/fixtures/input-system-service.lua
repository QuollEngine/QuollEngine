event = nil
key = nil
mods = nil

local input = game:get("Input")

input.onKeyPress:connect(function(e)
    event = "press"
    key = e.key
    mods = e.mods
end)

input.onKeyRelease:connect(function(e)
    event = "release"
    key = e.key
    mods = e.mods
end)

event = nil
key = nil
mods = nil

local input = game:get("Input")

input.on_key_press:connect(function(e)
    event = "press"
    key = e.key
    mods = e.mods
end)

input.on_key_release:connect(function(e)
    event = "release"
    key = e.key
    mods = e.mods
end)

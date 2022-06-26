function start()
end

function update()
end

-- Name component
name = ''

function name_get()
    name = entity.name.get()
end

function name_set()
    entity.name.set("Hello World")
end

function name_set_invalid()
    entity.name.set(nil)
    entity.name.set(true)
    entity.name.set(name_set)
    entity.name.set({})
end

-- Local transform component
local_position_x = 0
local_position_y = 0
local_position_z = 0
function local_transform_position_get()
    local_position_x, local_position_y, local_position_z = entity.local_transform.get_position()
end

function local_transform_position_set()
    entity.local_transform.set_position(2.5, 3.5, 0.2)
end

function local_transform_position_set_invalid()
    entity.local_transform.set_position(nil, 1.0, 1.0)
    entity.local_transform.set_position(1.0, nil, 1.0)
    entity.local_transform.set_position(1.0, 1.0, nil)

    entity.local_transform.set_position("string", 1.0, 1.0)
    entity.local_transform.set_position(1.0, "string", 1.0)
    entity.local_transform.set_position(1.0, 1.0, "string")

    entity.local_transform.set_position(true, 1.0, 1.0)
    entity.local_transform.set_position(1.0, true, 1.0)
    entity.local_transform.set_position(1.0, 1.0, true)

    entity.local_transform.set_position(local_transform_scale_set, 1.0, 1.0)
    entity.local_transform.set_position(1.0, local_transform_scale_set, 1.0)
    entity.local_transform.set_position(1.0, 1.0, local_transform_scale_set)

    entity.local_transform.set_position({}, 1.0, 1.0)
    entity.local_transform.set_position(1.0, {}, 1.0)
    entity.local_transform.set_position(1.0, 1.0, {})
end

local_scale_x = 0
local_scale_y = 0
local_scale_z = 0
function local_transform_scale_get()
    local_scale_x, local_scale_y, local_scale_z = entity.local_transform.get_scale()
end

function local_transform_scale_set()
    entity.local_transform.set_scale(2.5, 3.5, 0.2)
end

function local_transform_scale_set_invalid()
    entity.local_transform.set_scale(nil, 1.0, 1.0)
    entity.local_transform.set_scale(1.0, nil, 1.0)
    entity.local_transform.set_scale(1.0, 1.0, nil)

    entity.local_transform.set_scale("string", 1.0, 1.0)
    entity.local_transform.set_scale(1.0, "string", 1.0)
    entity.local_transform.set_scale(1.0, 1.0, "string")

    entity.local_transform.set_scale(true, 1.0, 1.0)
    entity.local_transform.set_scale(1.0, true, 1.0)
    entity.local_transform.set_scale(1.0, 1.0, true)

    entity.local_transform.set_scale(local_transform_scale_set, 1.0, 1.0)
    entity.local_transform.set_scale(1.0, local_transform_scale_set, 1.0)
    entity.local_transform.set_scale(1.0, 1.0, local_transform_scale_set)

    entity.local_transform.set_scale({}, 1.0, 1.0)
    entity.local_transform.set_scale(1.0, {}, 1.0)
    entity.local_transform.set_scale(1.0, 1.0, {})
end

audio_is_playing_flag = false
function audio_play()
    entity.audio.play()
end

function audio_is_playing()
    audio_is_playing_flag = entity.audio.is_playing()
end

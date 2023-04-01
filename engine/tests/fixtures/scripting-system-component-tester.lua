function start()
end

function update()
end

found_entity = -1

function entity_query_get_first_by_name_no_param()
   found_entity = entity_query.get_first_entity_by_name()
end

function entity_query_get_first_by_name_param_nil()
   found_entity = entity_query.get_first_entity_by_name(nil);
end

function entity_query_get_first_by_name_param_boolean()
   found_entity = entity_query.get_first_entity_by_name(true)
end

function entity_query_get_first_by_name_param_function()
   found_entity = entity_query.get_first_entity_by_name(entity_query_get_first_by_name)
end

function entity_query_get_first_by_name_param_table()
   found_entity = entity_query.get_first_entity_by_name({})
end

function entity_query_get_first_by_name()
    found_entity = entity_query.get_first_entity_by_name("Test")
end

-- Name 
name = ''

function name_get()
    name = entity.name:get()
end

function name_get_invalid()
    name = entity.name.get()
end

function name_set()
    entity.name:set("Hello World")
end

function name_set_invalid()
    entity.name.set("Test")
    entity.name:set(nil)
    entity.name:set(true)
    entity.name:set(name_set)
    entity.name:set({})
end

-- Local transform 
local_position_x = 0
local_position_y = 0
local_position_z = 0

function local_transform_position_get()
    local_position_x, local_position_y, local_position_z = entity.local_transform:get_position()
end

function local_transform_position_get_invalid()
    local_position_x, local_position_y, local_position_z = entity.local_transform.get_position()
end

function local_transform_position_set()
    entity.local_transform:set_position(2.5, 3.5, 0.2)
end

function local_transform_position_set_invalid()
    entity.local_transform.set_position(1.0, 1.0, 1.0)

    entity.local_transform:set_position(nil, 1.0, 1.0)
    entity.local_transform:set_position(1.0, nil, 1.0)
    entity.local_transform:set_position(1.0, 1.0, nil)

    entity.local_transform:set_position("string", 1.0, 1.0)
    entity.local_transform:set_position(1.0, "string", 1.0)
    entity.local_transform:set_position(1.0, 1.0, "string")

    entity.local_transform:set_position(true, 1.0, 1.0)
    entity.local_transform:set_position(1.0, true, 1.0)
    entity.local_transform:set_position(1.0, 1.0, true)

    entity.local_transform:set_position(local_transform_scale_set, 1.0, 1.0)
    entity.local_transform:set_position(1.0, local_transform_scale_set, 1.0)
    entity.local_transform:set_position(1.0, 1.0, local_transform_scale_set)

    entity.local_transform:set_position({}, 1.0, 1.0)
    entity.local_transform:set_position(1.0, {}, 1.0)
    entity.local_transform:set_position(1.0, 1.0, {})
end

local_scale_x = 0
local_scale_y = 0
local_scale_z = 0
function local_transform_scale_get()
    local_scale_x, local_scale_y, local_scale_z = entity.local_transform:get_scale()
end

function local_transform_scale_get_invalid()
    local_scale_x, local_scale_y, local_scale_z = entity.local_transform.get_scale()
end

function local_transform_scale_set()
    entity.local_transform:set_scale(2.5, 3.5, 0.2)
end

function local_transform_scale_set_invalid()
    entity.local_transform.set_scale(1.0, 1.0, 1.0)

    entity.local_transform:set_scale(nil, 1.0, 1.0)
    entity.local_transform:set_scale(1.0, nil, 1.0)
    entity.local_transform:set_scale(1.0, 1.0, nil)

    entity.local_transform:set_scale("string", 1.0, 1.0)
    entity.local_transform:set_scale(1.0, "string", 1.0)
    entity.local_transform:set_scale(1.0, 1.0, "string")

    entity.local_transform:set_scale(true, 1.0, 1.0)
    entity.local_transform:set_scale(1.0, true, 1.0)
    entity.local_transform:set_scale(1.0, 1.0, true)

    entity.local_transform:set_scale(local_transform_scale_set, 1.0, 1.0)
    entity.local_transform:set_scale(1.0, local_transform_scale_set, 1.0)
    entity.local_transform:set_scale(1.0, 1.0, local_transform_scale_set)

    entity.local_transform:set_scale({}, 1.0, 1.0)
    entity.local_transform:set_scale(1.0, {}, 1.0)
    entity.local_transform:set_scale(1.0, 1.0, {})
end


local_rotation_x = 0
local_rotation_y = 0
local_rotation_z = 0
function local_transform_rotation_get()
    local_rotation_x, local_rotation_y, local_rotation_z = entity.local_transform:get_rotation()
end

function local_transform_rotation_get_invalid()
    local_rotation_x, local_rotation_y, local_rotation_z = entity.local_transform.get_rotation()
end

function local_transform_rotation_set()
    entity.local_transform:set_rotation(35.0, 25.0, 45.0)
end

function local_transform_rotation_set_invalid()
    entity.local_transform.set_rotation(1.0, 1.0, 1.0)

    entity.local_transform:set_rotation(nil, 1.0, 1.0)
    entity.local_transform:set_rotation(1.0, nil, 1.0)
    entity.local_transform:set_rotation(1.0, 1.0, nil)

    entity.local_transform:set_rotation("string", 1.0, 1.0)
    entity.local_transform:set_rotation(1.0, "string", 1.0)
    entity.local_transform:set_rotation(1.0, 1.0, "string")

    entity.local_transform:set_rotation(true, 1.0, 1.0)
    entity.local_transform:set_rotation(1.0, true, 1.0)
    entity.local_transform:set_rotation(1.0, 1.0, true)

    entity.local_transform:set_rotation(local_transform_rotation_set, 1.0, 1.0)
    entity.local_transform:set_rotation(1.0, local_transform_rotation_set, 1.0)
    entity.local_transform:set_rotation(1.0, 1.0, local_transform_rotation_set)

    entity.local_transform:set_rotation({}, 1.0, 1.0)
    entity.local_transform:set_rotation(1.0, {}, 1.0)
    entity.local_transform:set_rotation(1.0, 1.0, {})
end

-- Rigid body
function rigid_body_apply_force()
    entity.rigid_body:apply_force(10.0, 0.2, 5.0);
end

function rigid_body_apply_force_invalid()
    entity.rigid_body.apply_force(0.0, 1.0, 1.0)

    entity.rigid_body:apply_force(nil, 0.0, 1.0)
    entity.rigid_body:apply_force(0.0, nil, 1.0)
    entity.rigid_body:apply_force(0.0, 1.0, nil)

    entity.rigid_body:apply_force("string", 0.0, 1.0)
    entity.rigid_body:apply_force(0.0, "string", 1.0)
    entity.rigid_body:apply_force(0.0, 1.0, "string")

    entity.rigid_body:apply_force(true, 0.0, 1.0)
    entity.rigid_body:apply_force(0.0, true, 1.0)
    entity.rigid_body:apply_force(0.0, 1.0, true)

    entity.rigid_body:apply_force(local_transform_scale_set, 0.0, 1.0)
    entity.rigid_body:apply_force(0.0, local_transform_scale_set, 1.0)
    entity.rigid_body:apply_force(0.0, 1.0, local_transform_scale_set)

    entity.rigid_body:apply_force({}, 0.0, 1.0)
    entity.rigid_body:apply_force(0.0, {}, 1.0)
    entity.rigid_body:apply_force(0.0, 1.0, {})
end

function rigid_body_apply_torque()
    entity.rigid_body:apply_torque(2.5, 3.5, 1.2)
end

function rigid_body_apply_torque_invalid()
    entity.rigid_body.apply_torque(1.0, 1.0, 1.0)

    entity.rigid_body:apply_torque(nil, 1.0, 1.0)
    entity.rigid_body:apply_torque(1.0, nil, 1.0)
    entity.rigid_body:apply_torque(1.0, 1.0, nil)

    entity.rigid_body:apply_torque("string", 1.0, 1.0)
    entity.rigid_body:apply_torque(1.0, "string", 1.0)
    entity.rigid_body:apply_torque(1.0, 1.0, "string")

    entity.rigid_body:apply_torque(true, 1.0, 1.0)
    entity.rigid_body:apply_torque(1.0, true, 1.0)
    entity.rigid_body:apply_torque(1.0, 1.0, true)

    entity.rigid_body:apply_torque(local_transform_scale_set, 1.0, 1.0)
    entity.rigid_body:apply_torque(1.0, local_transform_scale_set, 1.0)
    entity.rigid_body:apply_torque(1.0, 1.0, local_transform_scale_set)

    entity.rigid_body:apply_torque({}, 1.0, 1.0)
    entity.rigid_body:apply_torque(1.0, {}, 1.0)
    entity.rigid_body:apply_torque(1.0, 1.0, {})
end

function rigid_body_clear()
    entity.rigid_body:clear()
end

function rigid_body_clear_invalid()
    entity.rigid_body.clear()
end

-- Text
text = ''
text_line_height = -1

function text_get_text()
    text = entity.text:get_text()
end

function text_get_invalid_text()
    text = entity.name.get_text()
end

function text_set_text()
    entity.text:set_text("Hello World")
end

function text_set_text_invalid()
    entity.text.set_text(12)
    entity.text:set_text(nil)
    entity.text:set_text(true)
    entity.text:set_text(name_set)
    entity.text:set_text({})
end

function text_get_line_height()
    text_line_height = entity.text:get_line_height()
end

function text_get_line_height_invalid()
    text_line_height = entity.text.get_line_height()
end

function text_set_line_height()
    entity.text:set_line_height(12)
end

function text_set_line_height_invalid()
    entity.text.set_line_height("Test")
    entity.text:set_line_height(nil)
    entity.text:set_line_height(true)
    entity.text:set_line_height(name_set)
    entity.text:set_line_height({})
end

-- Audio 
audio_is_playing_flag = false
function audio_play()
    entity.audio:play()
end

function audio_play_invalid()
    entity.audio.play()
end

function audio_is_playing()
    audio_is_playing_flag = entity.audio:is_playing()
end

function audio_is_playing_invalid()
    audio_is_playing_flag = entity.audio.is_playing()
end

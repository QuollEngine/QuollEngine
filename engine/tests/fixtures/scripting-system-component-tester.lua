function expect_eq(a, b)
    ret = assert_native(a == b)
    if ret == false then
        msg = "Assertion failed: " .. tostring(a) .. " - " .. tostring(b)
        error(msg)
    end
end

function start()
end

function update()
end

found_entity = -1

-- Entity query: get first by name
function entity_query_get_first_by_name()
    found_entity = entity_query:get_first_entity_by_name("Test")
end

-- Entity query: delete entity
function entity_query_delete_entity()
    entity_query:delete_entity(entity)
end

created_entity = -1

-- Entity spawner
function entity_spawner_spawn_empty()
    created_entity = entity_spawner:spawn_empty()
end

function entity_spawner_spawn_prefab()
    created_entity = entity_spawner:spawn_prefab(1)
end

function entity_spawner_spawn_sprite()
    created_entity = entity_spawner:spawn_sprite(1)
end

-- Name 
name = ''

function name_get()
    name = entity.name:get()
end

function name_set()
    entity.name:set("Hello World")
end

function name_delete()
    entity.name:delete()
end

-- Local transform 
local_position_x = 0
local_position_y = 0
local_position_z = 0

function local_transform_position_get()
    local_position_x, local_position_y, local_position_z = entity.local_transform:get_position()
end

function local_transform_position_set()
    entity.local_transform:set_position(2.5, 3.5, 0.2)
end

local_scale_x = 0
local_scale_y = 0
local_scale_z = 0
function local_transform_scale_get()
    local_scale_x, local_scale_y, local_scale_z = entity.local_transform:get_scale()
end

function local_transform_scale_set()
    entity.local_transform:set_scale(2.5, 3.5, 0.2)
end

local_rotation_x = 0
local_rotation_y = 0
local_rotation_z = 0
function local_transform_rotation_get()
    local_rotation_x, local_rotation_y, local_rotation_z = entity.local_transform:get_rotation()
end

function local_transform_rotation_set()
    entity.local_transform:set_rotation(35.0, 25.0, 45.0)
end

function local_transform_delete()
    entity.local_transform:delete()
end

-- Rigid body
function rigid_body_set_default_params()
    entity.rigid_body:set_default_params();
end

mass = 0
function rigid_body_get_mass()
    mass = entity.rigid_body:get_mass()
end

function rigid_body_set_mass()
    entity.rigid_body:set_mass(2.5);
end

inertia_x = 0
inertia_y = 0
inertia_z = 0
function rigid_body_get_inertia()
    inertia_x, inertia_y, inertia_z = entity.rigid_body:get_inertia()
end

function rigid_body_set_inertia()
    entity.rigid_body:set_inertia(2.5, 2.5, 2.5)
end


is_gravity_applied = true
function rigid_body_is_gravity_applied()
    is_gravity_applied = entity.rigid_body:is_gravity_applied()
end

function rigid_body_apply_gravity()
    entity.rigid_body:apply_gravity(false)
end

function rigid_body_apply_force()
    entity.rigid_body:apply_force(10.0, 0.2, 5.0);
end

function rigid_body_apply_torque()
    entity.rigid_body:apply_torque(2.5, 3.5, 1.2)
end

function rigid_body_clear()
    entity.rigid_body:clear()
end

function rigid_body_delete()
    entity.rigid_body:delete()
end

-- Collidable
function collidable_set_default_material()
    entity.collidable:set_default_material();
end

static_friction = 0
function collidable_get_static_friction()
    static_friction = entity.collidable:get_static_friction()
end

function collidable_set_static_friction()
    entity.collidable:set_static_friction(2.5);
end

dynamic_friction = 0
function collidable_get_dynamic_friction()
    dynamic_friction = entity.collidable:get_dynamic_friction()
end

function collidable_set_dynamic_friction()
    entity.collidable:set_dynamic_friction(2.5);
end

restitution = 0
function collidable_get_restitution()
    restitution = entity.collidable:get_restitution()
end

function collidable_set_restitution()
    entity.collidable:set_restitution(2.5);
end

function collidable_set_box_geometry()
    entity.collidable:set_box_geometry(2.5, 2.5, 2.5)
end

function collidable_set_sphere_geometry()
    entity.collidable:set_sphere_geometry(2.5)
end

function collidable_set_capsule_geometry()
    entity.collidable:set_capsule_geometry(2.5, 3.5)
end

function collidable_set_plane_geometry()
    entity.collidable:set_plane_geometry()
end

sweep_output = nil
sweep_normal = nil
sweep_data = nil
function collidable_sweep()
    sweep_output, sweep_data = entity.collidable:sweep(0.0, 0.0, 0.0, 0.0)

    if sweep_output then
       sweep_normal = sweep_data.normal
    end
end

function collidable_delete()
    entity.collidable:delete()
end

-- Text
text = ''
text_line_height = -1

function text_get_text()
    text = entity.text:get_text()
end

function text_set_text()
    entity.text:set_text("Hello World")
end

function text_get_line_height()
    text_line_height = entity.text:get_line_height()
end

function text_set_line_height()
    entity.text:set_line_height(12)
end

function text_delete()
    entity.text:delete()
end

-- Animator
function animator_trigger()
   entity.animator:trigger("Move")
end

function animator_delete()
    entity.animator:delete()
end

-- Audio 
audio_is_playing_flag = false
function audio_play()
    entity.audio:play()
end

function audio_is_playing()
    audio_is_playing_flag = entity.audio:is_playing()
end

function audio_delete()
    entity.audio:delete()
end

-- Perspective lens
pr_near = 0
pr_far = 0
pr_sensor_width = 0
pr_sensor_height = 0
pr_focal_length = 0
pr_aperture = 0
pr_shutter_speed = 0
pr_sensitivity = 0

function perspective_lens_get()
  pr_near = entity.perspective_lens:get_near()
  pr_far = entity.perspective_lens:get_far()
  pr_sensor_width, pr_sensor_height = entity.perspective_lens:get_sensor_size()
  pr_focal_length = entity.perspective_lens:get_focal_length()
  pr_aperture = entity.perspective_lens:get_aperture()
  pr_shutter_speed = entity.perspective_lens:get_shutter_speed()
  pr_sensitivity = entity.perspective_lens:get_sensitivity()
end

function perspective_lens_set()
    entity.perspective_lens:set_near(0.004)
    entity.perspective_lens:set_far(4000.0)
    entity.perspective_lens:set_sensor_size(200.0, 200.0)
    entity.perspective_lens:set_focal_length(50.0)
    entity.perspective_lens:set_aperture(65.0)
    entity.perspective_lens:set_shutter_speed(2200.0)
    entity.perspective_lens:set_sensitivity(4000)
end

function perspective_lens_delete()
    entity.perspective_lens:delete()
end

-- Input map
input_command = 'test'
input_command_value = 'test'
input_command_value_x = 'test'
input_command_value_y = 'test'

--- get_command
function input_get_command()
    input_command = entity.input:get_command("Test")
end

--- get_value_boolean
function input_get_value_boolean()
    command = entity.input:get_command("Test")
    input_command_value = entity.input:get_value_boolean(command)
end

function input_get_value_boolean_non_existent_command()
    input_command_value = entity.input:get_value_boolean(10)
end

function input_get_value_boolean_no_member()
    command = entity.input:get_command("Test")
    input_command_value = entity.input.get_value_boolean(command)
end

function input_get_value_boolean_no_param()
    command = entity.input:get_command()
    input_command_value = entity.input:get_value_boolean()
end
 
--- get_value_axis_2d
function input_get_value_axis_2d()
    command = entity.input:get_command("Test")
    input_command_value_x, input_command_value_y = entity.input:get_value_axis_2d(command)
end

function input_get_value_axis_2d_non_existent_command()
    input_command_value_x, input_command_value_y = entity.input:get_value_axis_2d(10)
end
 
--- set_scheme
function input_set_scheme()
    entity.input:set_scheme("Test scheme")
end

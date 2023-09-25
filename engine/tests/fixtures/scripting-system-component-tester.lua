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
function entity_query_get_first_by_name_invalid()
   expect_eq(entity_query.get_first_entity_by_name(), nil)
   expect_eq(entity_query.get_first_entity_by_name(nil), nil)
   expect_eq(entity_query.get_first_entity_by_name(true), nil)
   expect_eq(entity_query.get_first_entity_by_name(entity_query_get_first_by_name), nil)
   expect_eq(entity_query.get_first_entity_by_name({}), nil)
   expect_eq(entity_query.get_first_entity_by_name(1), nil)
end

function entity_query_get_first_by_name()
    found_entity = entity_query.get_first_entity_by_name("Test")
end

-- Entity query: delete entity
function entity_query_delete_entity_invalid()
    entity_query.delete_entity()
    entity_query.delete_entity(nil)
    entity_query.delete_entity(true)
    entity_query.delete_entity({})
    entity_query.delete_entity("Test")
    entity_query.delete_entity({id=123123})
end

function entity_query_delete_entity_param_entity_table()
    entity_query.delete_entity(entity)
end

created_entity = -1

-- Entity spawner
function entity_spawner_spawn_empty()
    created_entity = entity_spawner.spawn_empty()
end

function entity_spawner_spawn_prefab_invalid()
    expect_eq(entity_spawner.spawn_prefab(), nil)
    expect_eq(entity_spawner.spawn_prefab())
    expect_eq(entity_spawner.spawn_prefab(nil))
    expect_eq(entity_spawner.spawn_prefab(true))
    expect_eq(entity_spawner.spawn_prefab({}))
    expect_eq(entity_spawner.spawn_prefab("Test"))
    expect_eq(entity_spawner.spawn_prefab(99999))
end

function entity_spawner_spawn_prefab()
    created_entity = entity_spawner.spawn_prefab(1)
end

function entity_spawner_spawn_sprite_invalid()
    expect_eq(entity_spawner.spawn_sprite())
    expect_eq(entity_spawner.spawn_sprite(nil))
    expect_eq(entity_spawner.spawn_sprite(true))
    expect_eq(entity_spawner.spawn_sprite({}))
    expect_eq(entity_spawner.spawn_sprite("Test"))
    expect_eq(entity_spawner.spawn_sprite(9999))
end

function entity_spawner_spawn_sprite()
    created_entity = entity_spawner.spawn_sprite(1)
end

-- Name 
name = ''

function name_get()
    name = entity.name:get()
end

function name_get_invalid()
    expect_eq(entity.name.get(), '')
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

function name_delete_invalid()
    entity.name.delete()
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

function local_transform_position_get_invalid()
    x, y, z = entity.local_transform.get_position()
    expect_eq(x, nil)
    expect_eq(y, nil)
    expect_eq(z, nil)
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
    x, y, z = entity.local_transform.get_scale()
    expect_eq(x, nil)
    expect_eq(y, nil)
    expect_eq(z, nil)
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
    x, y, z = entity.local_transform.get_rotation()
    expect_eq(x, nil)
    expect_eq(y, nil)
    expect_eq(z, nil)
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

function local_transform_delete_invalid()
    entity.local_transform.delete()
end

function local_transform_delete()
    entity.local_transform:delete()
end

-- Rigid body
function rigid_body_set_default_params()
    entity.rigid_body:set_default_params();
end

function rigid_body_set_default_params_invalid()
    entity.rigid_body.set_default_params()
end

mass = 0
function rigid_body_get_mass()
    mass = entity.rigid_body:get_mass()
end

function rigid_body_get_mass_invalid()
    expect_eq(entity.rigid_body.get_mass(), nil)
end

function rigid_body_set_mass()
    entity.rigid_body:set_mass(2.5);
end

function rigid_body_set_mass_invalid()
    entity.rigid_body.set_mass(0.0)
    entity.rigid_body:set_mass(nil)
    entity.rigid_body:set_mass("string")
    entity.rigid_body:set_mass(true)
    entity.rigid_body:set_mass(local_transform_scale_set)
    entity.rigid_body:set_mass({})
end

inertia_x = 0
inertia_y = 0
inertia_z = 0
function rigid_body_get_inertia()
    inertia_x, inertia_y, inertia_z = entity.rigid_body:get_inertia()
end

function rigid_body_get_inertia_invalid()
    x, y, x = entity.rigid_body.get_inertia()
    expect_eq(x, nil)
    expect_eq(y, nil)
    expect_eq(z, nil)
end

function rigid_body_set_inertia()
    entity.rigid_body:set_inertia(2.5, 2.5, 2.5)
end

function rigid_body_set_inertia_invalid()
    entity.rigid_body.set_inertia(0.0, 1.0, 1.0)

    entity.rigid_body:set_inertia(nil, 0.0, 1.0)
    entity.rigid_body:set_inertia(0.0, nil, 1.0)
    entity.rigid_body:set_inertia(0.0, 1.0, nil)

    entity.rigid_body:set_inertia("string", 0.0, 1.0)
    entity.rigid_body:set_inertia(0.0, "string", 1.0)
    entity.rigid_body:set_inertia(0.0, 1.0, "string")

    entity.rigid_body:set_inertia(true, 0.0, 1.0)
    entity.rigid_body:set_inertia(0.0, true, 1.0)
    entity.rigid_body:set_inertia(0.0, 1.0, true)

    entity.rigid_body:set_inertia(local_transform_scale_set, 0.0, 1.0)
    entity.rigid_body:set_inertia(0.0, local_transform_scale_set, 1.0)
    entity.rigid_body:set_inertia(0.0, 1.0, local_transform_scale_set)

    entity.rigid_body:set_inertia({}, 0.0, 1.0)
    entity.rigid_body:set_inertia(0.0, {}, 1.0)
    entity.rigid_body:set_inertia(0.0, 1.0, {})
end

is_gravity_applied = true
function rigid_body_is_gravity_applied()
    is_gravity_applied = entity.rigid_body:is_gravity_applied()
end

function rigid_body_is_gravity_applied_invalid()
    expect_eq(entity.rigid_body.is_gravity_applied(), nil)
end

function rigid_body_apply_gravity()
    entity.rigid_body:apply_gravity(false)
end

function rigid_body_apply_gravity_invalid()
    entity.rigid_body.apply_gravity(0.0)
    entity.rigid_body:apply_gravity(nil)
    entity.rigid_body:apply_gravity("string")
    entity.rigid_body:apply_gravity(2.5)
    entity.rigid_body:apply_gravity(local_transform_scale_set)
    entity.rigid_body:apply_gravity({})
end

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

function rigid_body_delete()
    entity.rigid_body:delete()
end

function rigid_body_delete_invalid()
    entity.rigid_body.delete()
end

-- Collidable
function collidable_set_default_material()
    entity.collidable:set_default_material();
end

function collidable_set_default_material_invalid()
    entity.collidable.set_default_material()
end

static_friction = 0
function collidable_get_static_friction()
    static_friction = entity.collidable:get_static_friction()
end

function collidable_get_static_friction_invalid()
    expect_eq(entity.collidable.get_static_friction(), nil)
end

function collidable_set_static_friction()
    entity.collidable:set_static_friction(2.5);
end

function collidable_set_static_friction_invalid()
    entity.collidable.set_static_friction(0.0)
    entity.collidable:set_static_friction(nil)
    entity.collidable:set_static_friction("string")
    entity.collidable:set_static_friction(true)
    entity.collidable:set_static_friction(local_transform_scale_set)
    entity.collidable:set_static_friction({})
end

dynamic_friction = 0
function collidable_get_dynamic_friction()
    dynamic_friction = entity.collidable:get_dynamic_friction()
end

function collidable_get_dynamic_friction_invalid()
    expect_eq(entity.collidable.get_dynamic_friction(), nil)
end

function collidable_set_dynamic_friction()
    entity.collidable:set_dynamic_friction(2.5);
end

function collidable_set_dynamic_friction_invalid()
    entity.collidable.set_dynamic_friction(0.0)
    entity.collidable:set_dynamic_friction(nil)
    entity.collidable:set_dynamic_friction("string")
    entity.collidable:set_dynamic_friction(true)
    entity.collidable:set_dynamic_friction(local_transform_scale_set)
    entity.collidable:set_dynamic_friction({})
end

restitution = 0
function collidable_get_restitution()
    restitution = entity.collidable:get_restitution()
end

function collidable_get_restitution_invalid()
    expect_eq(entity.collidable.get_restitution(), nil)
end

function collidable_set_restitution()
    entity.collidable:set_restitution(2.5);
end

function collidable_set_restitution_invalid()
    entity.collidable.set_restitution(0.0)
    entity.collidable:set_restitution(nil)
    entity.collidable:set_restitution("string")
    entity.collidable:set_restitution(true)
    entity.collidable:set_restitution(local_transform_scale_set)
    entity.collidable:set_restitution({})
end

function collidable_set_box_geometry()
    entity.collidable:set_box_geometry(2.5, 2.5, 2.5)
end

function collidable_set_box_geometry_invalid()
    entity.collidable.set_box_geometry(1.0, 1.0, 1.0)

    entity.collidable:set_box_geometry(nil, 1.0, 1.0)
    entity.collidable:set_box_geometry(1.0, nil, 1.0)
    entity.collidable:set_box_geometry(1.0, 1.0, nil)

    entity.collidable:set_box_geometry("string", 1.0, 1.0)
    entity.collidable:set_box_geometry(1.0, "string", 1.0)
    entity.collidable:set_box_geometry(1.0, 1.0, "string")

    entity.collidable:set_box_geometry(true, 1.0, 1.0)
    entity.collidable:set_box_geometry(1.0, true, 1.0)
    entity.collidable:set_box_geometry(1.0, 1.0, true)

    entity.collidable:set_box_geometry(local_transform_scale_set, 1.0, 1.0)
    entity.collidable:set_box_geometry(1.0, local_transform_scale_set, 1.0)
    entity.collidable:set_box_geometry(1.0, 1.0, local_transform_scale_set)

    entity.collidable:set_box_geometry({}, 1.0, 1.0)
    entity.collidable:set_box_geometry(1.0, {}, 1.0)
    entity.collidable:set_box_geometry(1.0, 1.0, {})
end

function collidable_set_sphere_geometry()
    entity.collidable:set_sphere_geometry(2.5)
end

function collidable_set_sphere_geometry_invalid()
    entity.collidable.set_box_geometry(0.0)
    entity.collidable:set_box_geometry(nil)
    entity.collidable:set_box_geometry("string")
    entity.collidable:set_box_geometry(true)
    entity.collidable:set_box_geometry(local_transform_scale_set)
    entity.collidable:set_box_geometry({})
end

function collidable_set_capsule_geometry()
    entity.collidable:set_capsule_geometry(2.5, 3.5)
end

function collidable_set_capsule_geometry_invalid()
    entity.collidable.set_box_geometry(1.0, 1.0)

    entity.collidable:set_box_geometry(nil, 1.0)
    entity.collidable:set_box_geometry(1.0, nil)

    entity.collidable:set_box_geometry("string", 1.0)
    entity.collidable:set_box_geometry(1.0, "string")

    entity.collidable:set_box_geometry(true, 1.0)
    entity.collidable:set_box_geometry(1.0, true)

    entity.collidable:set_box_geometry(local_transform_scale_set, 1.0)
    entity.collidable:set_box_geometry(1.0, local_transform_scale_set)

    entity.collidable:set_box_geometry({}, 1.0)
    entity.collidable:set_box_geometry(1.0, {})
end

function collidable_set_plane_geometry()
    entity.collidable:set_plane_geometry()
end

function collidable_set_plane_geometry_invalid()
    entity.collidable.set_plane_geometry()
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

function collidable_sweep_invalid()
    function expect_eq_c(a, b)
        expect_eq(a, false)
        expect_eq(b, nil)
    end

    expect_eq_c(entity.collidable.sweep(0.0, 1.0, 0.0, 2.0))

    expect_eq_c(entity.collidable:sweep())
    expect_eq_c(entity.collidable:sweep(0.0))
    expect_eq_c(entity.collidable:sweep(0.0, 0.0))
    expect_eq_c(entity.collidable:sweep(0.0, 0.0, 0.0))

    invalid_vars = {true, "test", {}, local_transform_scale_set, nil}

    for _, var in ipairs(invalid_vars) do
        expect_eq_c(entity.collidable:sweep(var, 1.0, 0.0, 2.0))
        expect_eq_c(entity.collidable:sweep(0.0, var, 0.0, 2.0))
        expect_eq_c(entity.collidable:sweep(0.0, 0.0, var, 2.0))
        expect_eq_c(entity.collidable:sweep(0.0, 0.0, 0.0, var))
    end
end

function collidable_delete()
    entity.collidable:delete()
end

function collidable_delete_invalid()
    entity.collidable.delete()
end

-- Text
text = ''
text_line_height = -1

function text_get_text()
    text = entity.text:get_text()
end

function text_get_text_invalid()
    expect_eq(entity.text.get_text(), '')
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
    expect_eq(entity.text.get_line_height(), 0.0)
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

function text_delete_invalid()
    entity.text.delete()
end

function text_delete()
    entity.text:delete()
end

-- Animator
function animator_trigger_invalid()
    entity.animator.trigger(12)
    entity.animator:trigger(nil)
    entity.animator:trigger(true)
    entity.animator:trigger(name_set)
    entity.animator:trigger({})
end

function animator_trigger()
   entity.animator:trigger("Move")
end

function animator_delete_invalid()
    entity.animator.delete()
end

function animator_delete()
    entity.animator:delete()
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
    expect_eq(entity.audio.is_playing(), false)
end

function audio_delete_invalid()
    entity.audio.delete()
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

function perspective_lens_get_invalid()
    expect_eq(entity.perspective_lens.get_near(), nil)
    expect_eq(entity.perspective_lens.get_far(), nil)
    w, h = entity.perspective_lens.get_sensor_size()
    expect_eq(w, nil)
    expect_eq(h, nil)

    expect_eq(entity.perspective_lens.get_focal_length(), nil)
    expect_eq(entity.perspective_lens.get_aperture(), nil)
    expect_eq(entity.perspective_lens.get_shutter_speed(), nil)
    expect_eq(entity.perspective_lens.get_sensitivity(), nil)
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

function perspective_lens_set_invalid_param(value)
    entity.perspective_lens:set_near(value)
    entity.perspective_lens:set_far(value)
    entity.perspective_lens:set_sensor_size(value, 200.0)
    entity.perspective_lens:set_sensor_size(200.0, value)
    entity.perspective_lens:set_focal_length(value)
    entity.perspective_lens:set_aperture(value)
    entity.perspective_lens:set_shutter_speed(value)
    entity.perspective_lens:set_sensitivity(value)
end

function perspective_lens_set_invalid()
    entity.perspective_lens.set_near(0.004)
    entity.perspective_lens.set_far(4000.0)
    entity.perspective_lens.set_sensor_size(200.0, 200.0)
    entity.perspective_lens.set_focal_length(50.0)
    entity.perspective_lens.set_aperture(65.0)
    entity.perspective_lens.set_shutter_speed(2200.0)
    entity.perspective_lens.set_sensitivity(4000)

    entity.perspective_lens:set_sensor_size(200.0)

    perspective_lens_set_invalid_param(nil)
    perspective_lens_set_invalid_param("string")
    perspective_lens_set_invalid_param(true)
    perspective_lens_set_invalid_param(perspective_lens_set)
    perspective_lens_set_invalid_param({})
end

function perspective_lens_delete()
    entity.perspective_lens:delete()
end

function perspective_lens_delete_invalid()
    entity.perspective_lens.delete()
end

-- Input map
input_command = nil
input_command_value = nil
input_command_value_x = nil
input_command_value_y = nil

--- get_command
function input_get_command()
    input_command = entity.input:get_command("Test")
end

function input_get_command_invalid()
    expect_eq(entity.input.get_command("Test"), nil)
    expect_eq( entity.input:get_command())
    expect_eq( entity.input:get_command(nil))
    expect_eq( entity.input:get_command(10))
    expect_eq( entity.input:get_command(true))
    expect_eq( entity.input:get_command(entity_query_get_first_by_name))
    expect_eq( entity.input:get_command({}))
end
 
--- get_value_boolean
function input_get_value_boolean()
    command = entity.input:get_command("Test")
    input_command_value = entity.input:get_value_boolean(command)
end

function input_get_value_boolean_non_existent_command()
    input_command_value = entity.input.get_value_boolean(10)
end

function input_get_value_boolean_no_member()
    command = entity.input:get_command("Test")
    input_command_value = entity.input.get_value_boolean(command)
end

function input_get_value_boolean_no_param()
    command = entity.input:get_command()
    input_command_value = entity.input:get_value_boolean()
end
 
function input_get_value_boolean_nil()
    command = entity.input:get_command()
    input_command_value = entity.input:get_value_boolean(nil)
end
 
function input_get_value_boolean_string()
    command = entity.input:get_command()
    input_command_value = entity.input:get_value_boolean("test")
end
 
function input_get_value_boolean_function()
    command = entity.input:get_command()
    input_command_value = entity.input:get_value_boolean(entity_query_get_first_by_name)
end
 
function input_get_value_boolean_table()
    command = entity.input:get_command()
    input_command_value = entity.input:get_value_boolean({})
end

--- get_value_axis_2d
function input_get_value_axis_2d()
    command = entity.input:get_command("Test")
    input_command_value_x, input_command_value_y = entity.input:get_value_axis_2d(command)
end

function input_get_value_axis_2d_no_member()
    command = entity.input:get_command("Test")
    input_command_value_x, input_command_value_y = entity.input.get_value_axis_2d(command)
end

function input_get_value_axis_2d_non_existent_command()
    input_command_value_x, input_command_value_y = entity.input.get_value_axis_2d(10)
end

function input_get_value_axis_2d_no_param()
    command = entity.input:get_command()
    input_command_value_x, input_command_value_y = entity.input:get_value_axis_2d()
end
 
function input_get_value_axis_2d_nil()
    command = entity.input:get_command()
    input_command_value_x, input_command_value_y = entity.input:get_value_axis_2d(nil)
end
 
function input_get_value_axis_2d_string()
    command = entity.input:get_command()
    input_command_value_x, input_command_value_y = entity.input:get_value_axis_2d("test")
end
 
function input_get_value_axis_2d_function()
    command = entity.input:get_command()
    input_command_value_x, input_command_value_y = entity.input:get_value_axis_2d(entity_query_get_first_by_name)
end
 
function input_get_value_axis_2d_table()
    command = entity.input:get_command()
    input_command_value_x, input_command_value_y = entity.input:get_value_axis_2d({})
end
 
--- set_scheme
function input_set_scheme()
    entity.input:set_scheme("Test scheme")
end

function input_set_scheme_invalid()
    entity.input.set_scheme("Test scheme")
    entity.input:set_scheme()
    entity.input:set_scheme(10)
    entity.input:set_scheme(true)
    entity.input:set_scheme(nil)
    entity.input:set_scheme({})
    entity.input:set_scheme(entity_query_get_first_by_name)
end

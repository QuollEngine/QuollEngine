event = nil
a = nil
b = nil

local physics = game:get("Physics")

physics.on_collision_start:connect(function(e)
    event = "start"
    a = e.a
    b = e.b

    assert_native(e.a == entity)
end)

physics.on_collision_end:connect(function(e)
    event = "end"
    a = e.a
    b = e.b

    assert_native(e.a == entity)
end)

value = 0
event = 0
target = -1
key_value = -1
key_mods = -1
global_dt = 0.0

value = value - 1

updater_fn = game.on_update:connect(function(dt)
  value = value + 1
  global_dt = dt
end)

function disconnect_updater()
  updater_fn:disconnect()
end

function on_collision_start(collision)
    event = 1
    target = collision.target
end

function on_collision_end(collision)
    event = 2
    target = collision.target
end

function on_key_press(key)
    event = 3
    key_value = key.key
    key_mods = key.mods
end

function on_key_release(key)
    event = 4
    key_value = key.key
    key_mods = key.mods
end

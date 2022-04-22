value = 0
event = 0
target = -1
key_value = -1

function start()
    value = value - 1
end

function update()
    value = value + 1
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
end

function on_key_release(key)
    event = 4
    key_value = key.key
end

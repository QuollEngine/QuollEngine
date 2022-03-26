value = 0
event = 0

function start()
    value = value - 1
end

function update()
    value = value + 1
end

function on_collision_start()
    event = 1
end

function on_collision_end()
    event = 2
end

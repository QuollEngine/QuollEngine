velocity = 0.0

wall_line_left = 5.0 - 0.1;
wall_line_right = -5.0 + 0.1;
paddle_width = 1.0;

function start()
    entity.local_transform.set_position(0.0, 0.0, -3.0)
    entity.local_transform.set_scale(1.0, 0.2, 0.1)
end

function update(dt)
    x, y, z = entity.local_transform.get_position()
    new_x = x + velocity * dt

    right_bound = new_x - paddle_width
    left_bound = new_x + paddle_width

    if ((right_bound > wall_line_right) and (left_bound < wall_line_left)) then
        entity.local_transform.set_position(new_x, y, z)
    end
end

function on_key_press(e)
    if (e.key == 65) then
        velocity = 5.0
    elseif (e.key == 68) then
        velocity = -5.0
    end
end

function on_key_release()
    velocity = 0.0
end

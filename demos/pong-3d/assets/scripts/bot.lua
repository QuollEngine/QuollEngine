velocity = 0.0

wall_line_left = 5.0 - 0.1;
wall_line_right = -5.0 + 0.1;
paddle_width = 1.0;

ball = nil

function start()
    entity.local_transform:set_position(0.0, 0.0, 3.0)
    entity.local_transform:set_scale(1.0, 0.2, 0.1)

    ball = entity_query.get_first_entity_by_name("Ball")
end

function update(dt)
    x, y, z = entity.local_transform:get_position()
    bx, by, bz = ball.local_transform:get_position()

    if (math.abs(x - bx) < 0.2) then
        velocity = 0.0
    elseif (x > bx) then
        velocity = -5.0
    else
        velocity = 5.0
    end

    new_x = x + velocity * dt

    right_bound = new_x - paddle_width
    left_bound = new_x + paddle_width

    if ((right_bound > wall_line_right) and (left_bound < wall_line_left)) then
        entity.local_transform:set_position(new_x, y, z)
    end
end

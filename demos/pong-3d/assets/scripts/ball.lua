player_wall = nil
bot_wall = nil

function start()
    entity.rigid_body:apply_force(0.0, 0.0, 800.0)
    entity.rigid_body:apply_torque(10.0, 0.0, 0.0)

    player_wall = entity_query.get_first_entity_by_name("Player wall")
    bot_wall = entity_query.get_first_entity_by_name("Bot wall")
end

function update()
end

function on_collision_start(collision)
    if collision.target == player_wall.id then
        entity.local_transform:set_position(0.0, 0.0, 0.0)
    elseif collision.target == bot_wall.id then
        entity.local_transform:set_position(0.0, 0.0, 0.0)
    end
end

value = 0
global_dt = 0.0

value = value - 1

updater_fn = game.onUpdate:connect(function(dt)
  value = value + 1
  global_dt = dt
end)

function disconnect_updater()
  updater_fn:disconnect()
end


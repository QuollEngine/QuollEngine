state = 'start'

game.on_update:connect(function()
    state = 'update'
end)

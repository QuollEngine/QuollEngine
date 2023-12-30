var_string = inputVars.register('string_value', inputVars.types.String)
var_prefab = inputVars.register('prefab_value', inputVars.types.AssetPrefab)

state = 'start'

game.onUpdate:connect(function()
    state = 'update'
end)

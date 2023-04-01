var_string = input_vars.register('string_value', input_vars.types.String)
var_prefab = input_vars.register('prefab_value', input_vars.types.AssetPrefab)

state = 'none'

function start()
    state = 'start'
end

function update()
    state = 'update'
end

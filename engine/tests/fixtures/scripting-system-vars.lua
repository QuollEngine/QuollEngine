var_string = input_vars.register('string_value', input_vars.types.String)
var_prefab = input_vars.register('prefab_value', input_vars.types.AssetPrefab)

global_vars = input_vars

function start()
end

function update()
    -- After script is started the 
    -- `input_vars` global variable
    -- will be set to null; so, this
    -- function is used to assert
    -- this behavior
    global_vars = input_vars
end

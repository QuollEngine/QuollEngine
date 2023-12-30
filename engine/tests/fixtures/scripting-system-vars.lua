var_string = inputVars.register('string_value', inputVars.types.String)
var_prefab = inputVars.register('prefab_value', inputVars.types.AssetPrefab)
var_texture = inputVars.register('texture_value', inputVars.types.AssetTexture)

global_vars = inputVars

function update()
    -- After script is started the 
    -- `inputVars` global variable
    -- will be set to null; so, this
    -- function is used to assert
    -- this behavior
    global_vars = inputVars
end

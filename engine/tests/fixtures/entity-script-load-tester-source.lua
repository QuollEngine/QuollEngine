local entityQuery = game:get("EntityQuery")

local target = entityQuery:getFirstEntityByName("target")
retrievedScriptValue = target.script:get("scriptGlobalValue")
nonExistentScriptValue = target.script:get("nonExistentValue")
target.script:set("scriptAnotherValue", "yes")

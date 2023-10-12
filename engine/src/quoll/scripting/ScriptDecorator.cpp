#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityQueryLuaTable.h"
#include "quoll/entity/EntitySpawnerLuaTable.h"
#include "quoll/core/NameLuaTable.h"
#include "quoll/physics/RigidBodyLuaTable.h"
#include "quoll/physics/CollidableLuaTable.h"
#include "quoll/audio/AudioLuaTable.h"
#include "quoll/scene/TransformLuaTable.h"
#include "quoll/scene/PerspectiveLensLuaTable.h"
#include "quoll/text/TextLuaTable.h"
#include "quoll/animation/AnimatorLuaTable.h"
#include "quoll/input/InputMapLuaTable.h"
#include "quoll/physics/CollisionHitLuaTable.h"
#include "lua/Math.h"

#include "LuaMessages.h"
#include "LuaHeaders.h"
#include "ScriptDecorator.h"
#include "ScriptLogger.h"

#include "EntityTable.h"

namespace quoll {

void ScriptDecorator::attachToScope(sol::state_view state, Entity entity,
                                    ScriptGlobals &scriptGlobals) {
  LuaMath::create(state);
  CollisionHitLuaTable::create(state);
  EntityTable::create(state);
  EntitySpawnerLuaTable::create(state);
  EntityQueryLuaTable::create(state);

  state["entity"] = EntityTable(entity, scriptGlobals);
  state["entity_spawner"] = EntitySpawnerLuaTable(scriptGlobals);
  state["entity_query"] = EntityQueryLuaTable(scriptGlobals);

  createScriptLogger(state);
}

void ScriptDecorator::attachVariableInjectors(
    sol::state_view state,
    std::unordered_map<String, LuaScriptInputVariable> &variables) {
  auto inputVars = state.create_named_table("input_vars");
  inputVars["register"] = [&variables](String name, uint32_t type)
      -> std::variant<String, uint32_t, sol::nil_t> {
    if (type >= static_cast<uint32_t>(LuaScriptVariableType::Invalid)) {
      //     scope.error("Variable \"" + name + "\" has invalid type");
      return sol::nil;
    }

    auto value = variables.at(name);
    if (value.isType(LuaScriptVariableType::String)) {
      return value.get<String>();
    }

    if (value.isType(LuaScriptVariableType::AssetPrefab)) {
      return static_cast<uint32_t>(value.get<PrefabAssetHandle>());
    }

    if (value.isType(LuaScriptVariableType::AssetTexture)) {
      return static_cast<uint32_t>(value.get<TextureAssetHandle>());
    }

    return sol::nil;
  };

  inputVars["types"] = state.create_table_with(
      "Invalid", LuaScriptVariableType::Invalid,          //
      "String", LuaScriptVariableType::String,            //
      "AssetPrefab", LuaScriptVariableType::AssetPrefab,  //
      "AssetTexture", LuaScriptVariableType::AssetTexture //
  );
}

void ScriptDecorator::removeVariableInjectors(sol::state_view state) {
  state["input_vars"] = sol::nil;
}

} // namespace quoll

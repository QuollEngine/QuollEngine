#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/core/MathLuaTable.h"
#include "quoll/entity/EntityQueryLuaTable.h"
#include "quoll/entity/EntitySpawnerLuaTable.h"
#include "quoll/entity/EntityLuaTable.h"
#include "quoll/physics/CollisionHitLuaTable.h"
#include "quoll/ui/UILuaTable.h"
#include "quoll/logger/UserLoggerLuaTable.h"

#include "LuaHeaders.h"
#include "ScriptDecorator.h"
#include "ScriptSignal.h"

#include "GameLuaTable.h"

namespace quoll::lua {

void ScriptDecorator::attachToScope(sol::state_view state, Entity entity,
                                    ScriptGlobals scriptGlobals) {
  MathLuaTable::create(state);
  CollisionHitLuaTable::create(state);
  EntityLuaTable::create(state);
  EntitySpawnerLuaTable::create(state);
  EntityQueryLuaTable::create(state);
  UILuaTable::create(state);
  GameLuaTable::create(state);
  UserLoggerLuaTable::create(state);

  state["entity"] = EntityLuaTable(entity, scriptGlobals);
  state["entity_spawner"] = EntitySpawnerLuaTable(scriptGlobals);
  state["entity_query"] = EntityQueryLuaTable(scriptGlobals);
  state["game"] = GameLuaTable(entity, scriptGlobals);

  createScriptSignalTables(state);
}

void ScriptDecorator::attachVariableInjectors(
    sol::state_view state,
    std::unordered_map<String, LuaScriptInputVariable> &variables) {
  auto inputVars = state.create_named_table("input_vars");
  inputVars["register"] =
      [&variables](String name,
                   u32 type) -> std::variant<String, u32, sol::nil_t> {
    if (type >= static_cast<u32>(LuaScriptVariableType::Invalid)) {
      //     scope.error("Variable \"" + name + "\" has invalid type");
      return sol::nil;
    }

    auto value = variables.at(name);
    if (value.isType(LuaScriptVariableType::String)) {
      return value.get<String>();
    }

    if (value.isType(LuaScriptVariableType::AssetPrefab)) {
      return static_cast<u32>(value.get<PrefabAssetHandle>());
    }

    if (value.isType(LuaScriptVariableType::AssetTexture)) {
      return static_cast<u32>(value.get<TextureAssetHandle>());
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

} // namespace quoll::lua

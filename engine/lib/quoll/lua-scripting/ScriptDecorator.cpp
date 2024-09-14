#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/MathLuaTable.h"
#include "quoll/entity/EntityLuaTable.h"
#include "quoll/entity/EntityQueryLuaTable.h"
#include "quoll/entity/EntitySpawnerLuaTable.h"
#include "quoll/physics/CollisionHitLuaTable.h"
#include "quoll/signals/SignalLuaTable.h"
#include "GameLuaTable.h"
#include "LuaHeaders.h"
#include "ScriptDecorator.h"

namespace quoll::lua {

static void logMessages(LogStream &stream, sol::state_view state,
                        sol::variadic_args args) {
  for (auto arg : args) {
    stream << state["tostring"](arg.get<sol::object>()).get<String>() << "\t";
  }
}

void ScriptDecorator::attachToScope(sol::state_view state, Entity entity,
                                    ScriptGlobals scriptGlobals) {
  MathLuaTable::create(state);
  CollisionHitLuaTable::create(state, scriptGlobals);
  EntityLuaTable::create(state);
  GameLuaTable::create(state);
  SignalLuaTable::create(state);

  state.set_function("print", [state](sol::variadic_args args) {
    auto log = Engine::getUserLogger().debug();
    logMessages(log, state, args);
  });

  state["entity"] = EntityLuaTable(entity, scriptGlobals);
  state["game"] = GameLuaTable(entity, scriptGlobals);
}

void ScriptDecorator::attachVariableInjectors(
    sol::state_view state,
    std::unordered_map<String, LuaScriptInputVariable> &variables) {
  auto inputVars = state.create_named_table("inputVars");
  inputVars["register"] = [&variables](String name, u32 type)
      -> std::variant<String, AssetHandleType, sol::nil_t> {
    if (type >= static_cast<u32>(LuaScriptVariableType::Invalid)) {
      //     scope.error("Variable \"" + name + "\" has invalid type");
      return sol::nil;
    }

    auto value = variables.at(name);
    if (value.isType(LuaScriptVariableType::String)) {
      return value.get<String>();
    }

    if (value.isType(LuaScriptVariableType::AssetPrefab)) {
      return value.get<AssetRef<PrefabAsset>>().handle().getRawId();
    }

    if (value.isType(LuaScriptVariableType::AssetTexture)) {
      return value.get<AssetRef<TextureAsset>>().handle().getRawId();
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
  state["inputVars"] = sol::nil;
}

} // namespace quoll::lua

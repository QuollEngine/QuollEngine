#include "quoll/core/Base.h"
#include "InputVars.h"

namespace quoll::lua {

InputVars::InputVars(
    std::unordered_map<String, LuaScriptInputVariable> &variables)
    : mVariables(variables) {}

std::variant<String, u32> InputVars::registerVar(String name,
                                                 LuaScriptVariableType type) {
  if (static_cast<u32>(type) >=
      static_cast<u32>(LuaScriptVariableType::Invalid)) {
    // scope.error("Variable \"" + name + "\" has invalid type");
    return 0u;
  }

  auto value = mVariables.at(name);
  if (value.isType(LuaScriptVariableType::String)) {
    return value.get<String>();
  }

  if (value.isType(LuaScriptVariableType::AssetPrefab)) {
    return value.get<AssetRef<PrefabAsset>>().handle().getRawId();
  }

  if (value.isType(LuaScriptVariableType::AssetTexture)) {
    return value.get<AssetRef<TextureAsset>>().handle().getRawId();
  }

  return 0u;
}

void InputVars::create(sol::state_view state) {
  auto usertype =
      state.new_usertype<InputVars>("InputVars", sol::no_constructor);
  usertype["register"] = &InputVars::registerVar;
  usertype["types"] = state.create_table_with(
      "Invalid", LuaScriptVariableType::Invalid, "String",
      LuaScriptVariableType::String, "AssetPrefab",
      LuaScriptVariableType::AssetPrefab, "AssetTexture",
      LuaScriptVariableType::AssetTexture);
}

} // namespace quoll::lua

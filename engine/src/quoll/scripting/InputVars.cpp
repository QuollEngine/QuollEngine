#include "quoll/core/Base.h"
#include "InputVars.h"

namespace quoll {

InputVars::InputVars(
    std::unordered_map<String, LuaScriptInputVariable> &variables)
    : mVariables(variables) {}

std::variant<String, uint32_t>
InputVars::registerVar(String name, LuaScriptVariableType type) {
  if (static_cast<uint32_t>(type) >=
      static_cast<uint32_t>(LuaScriptVariableType::Invalid)) {
    // scope.error("Variable \"" + name + "\" has invalid type");
    return 0u;
  }

  auto value = mVariables.at(name);
  if (value.isType(LuaScriptVariableType::String)) {
    return value.get<String>();
  }

  if (value.isType(LuaScriptVariableType::AssetPrefab)) {
    return static_cast<uint32_t>(value.get<PrefabAssetHandle>());
  }

  if (value.isType(LuaScriptVariableType::AssetTexture)) {
    return static_cast<uint32_t>(value.get<TextureAssetHandle>());
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

} // namespace quoll

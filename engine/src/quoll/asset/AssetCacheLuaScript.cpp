#include "quoll/core/Base.h"
#include "quoll/lua-scripting/Interpreter.h"
#include "quoll/lua-scripting/NoopMetatable.h"
#include "AssetCache.h"

namespace quoll {

static std::vector<u8> readFileIntoBuffer(std::ifstream &stream) {
  std::ostringstream ss;
  ss << stream.rdbuf();
  const std::string &s = ss.str();
  std::vector<u8> bytes(s.begin(), s.end());

  return bytes;
}

static void injectInputVarsInterface(sol::state &state, LuaScriptAsset &data) {
  auto inputVars = state.create_named_table("inputVars");
  auto *luaState = state.lua_state();
  inputVars["register"] = [&data, luaState](String name, u32 type) {
    if (type >= static_cast<u32>(LuaScriptVariableType::Invalid)) {
      luaL_error(luaState, "Variable \"%s\" has invalid type", name.c_str());
      return sol::nil;
    }

    if (data.variables.find(name) != data.variables.end()) {
      luaL_error(luaState, "Variable \"%s\" already registered", name.c_str());
      return sol::nil;
    }

    LuaScriptVariable var{};
    var.name = name;
    var.type = static_cast<LuaScriptVariableType>(type);
    data.variables.insert({name, var});

    return sol::nil;
  };

  inputVars["types"] = state.create_table_with(
      "Invalid", LuaScriptVariableType::Invalid,          //
      "String", LuaScriptVariableType::String,            //
      "AssetPrefab", LuaScriptVariableType::AssetPrefab,  //
      "AssetTexture", LuaScriptVariableType::AssetTexture //
  );
}

Result<LuaScriptAsset> AssetCache::loadLuaScript(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  std::ifstream stream(filePath);

  if (!stream.good()) {
    return Error("File cannot be opened for reading: " + filePath.string());
  }

  auto meta = getAssetMeta(uuid);

  LuaScriptAsset asset;
  asset.bytes = readFileIntoBuffer(stream);

  stream.close();

  lua::Interpreter interpreter;

  sol::state state;
  lua::NoopMetatable::create(state);
  state["entity"] = lua::NoopMetatable{};
  state["table"] = lua::NoopMetatable{};
  state["game"] = lua::NoopMetatable{};
  state["print"] = [](sol::variadic_args) {};

  injectInputVarsInterface(state, asset);

  auto *luaState = state.lua_state();
  bool success = interpreter.evaluate(asset.bytes, luaState);

  if (!success) {
    const auto *message = lua_tostring(luaState, -1);
    return Error(message);
  }

  return asset;
}

} // namespace quoll

#pragma once

namespace quoll {

enum class LuaScriptVariableType { String, AssetPrefab, AssetTexture, Invalid };

struct LuaScriptVariable {
  LuaScriptVariableType type{LuaScriptVariableType::Invalid};

  String name;
};

struct LuaScriptAsset {
  std::vector<u8> bytes;

  std::unordered_map<String, LuaScriptVariable> variables;
};

} // namespace quoll

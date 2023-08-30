#pragma once

namespace quoll {

enum class LuaScriptVariableType { String, AssetPrefab, AssetTexture, Invalid };

/**
 * @brief Lua script variable
 */
struct LuaScriptVariable {
  /**
   * Variable type
   */
  LuaScriptVariableType type{LuaScriptVariableType::Invalid};

  /**
   * Variable name
   */
  String name;
};

/**
 * @brief Lua script asset data
 */
struct LuaScriptAsset {
  /**
   * Bytes that represent the script
   */
  std::vector<uint8_t> bytes;

  /**
   * Variables
   */
  std::unordered_map<String, LuaScriptVariable> variables;
};

} // namespace quoll

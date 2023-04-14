#pragma once

#include "liquid/asset/Asset.h"
#include "liquid/asset/LuaScriptAsset.h"

namespace liquid {

/**
 * @brief Lua script input variable value
 */
class LuaScriptInputVariable {
public:
  /**
   * @brief Create empty variable
   */
  LuaScriptInputVariable() = default;

  /**
   * @brief Create string variable value
   *
   * @param value String value
   */
  LuaScriptInputVariable(String value);

  /**
   * @brief Create prefab asset handle variable value
   *
   * @param value Prefab value
   */
  LuaScriptInputVariable(PrefabAssetHandle value);

  /**
   * @brief Get value
   *
   * @tparam TValue Value type
   * @return Value
   */
  template <class TValue> inline const TValue &get() const {
    return std::get<TValue>(mValue);
  }

  /**
   * @brief Check if type matches variable type
   *
   * @param type Type
   * @retval true Type matches variable type
   * @retval false Type does not match variable type
   */
  inline bool isType(LuaScriptVariableType type) const { return mType == type; }

private:
  LuaScriptVariableType mType = LuaScriptVariableType::Invalid;
  std::variant<String, PrefabAssetHandle> mValue;
};

} // namespace liquid

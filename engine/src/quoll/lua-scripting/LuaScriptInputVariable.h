#pragma once

#include "quoll/asset/Asset.h"
#include "LuaScriptAsset.h"

namespace quoll {

class LuaScriptInputVariable {
public:
  LuaScriptInputVariable() = default;

  LuaScriptInputVariable(String value);

  LuaScriptInputVariable(PrefabAssetHandle value);

  LuaScriptInputVariable(TextureAssetHandle value);

  template <class TValue> inline const TValue &get() const {
    return std::get<TValue>(mValue);
  }

  inline bool isType(LuaScriptVariableType type) const { return mType == type; }

private:
  LuaScriptVariableType mType = LuaScriptVariableType::Invalid;
  std::variant<String, PrefabAssetHandle, TextureAssetHandle> mValue;
};

} // namespace quoll

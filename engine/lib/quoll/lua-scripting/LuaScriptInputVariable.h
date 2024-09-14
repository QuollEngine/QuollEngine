#pragma once

#include "quoll/renderer/TextureAsset.h"
#include "quoll/scene/PrefabAsset.h"
#include "LuaScriptAsset.h"

namespace quoll {

class LuaScriptInputVariable {
public:
  LuaScriptInputVariable() = default;

  LuaScriptInputVariable(String value);

  LuaScriptInputVariable(AssetRef<PrefabAsset> value);

  LuaScriptInputVariable(AssetRef<TextureAsset> value);

  template <class TValue> inline const TValue &get() const {
    return std::get<TValue>(mValue);
  }

  inline bool isType(LuaScriptVariableType type) const { return mType == type; }

private:
  LuaScriptVariableType mType = LuaScriptVariableType::Invalid;
  std::variant<String, AssetRef<PrefabAsset>, AssetRef<TextureAsset>> mValue;
};

} // namespace quoll

#include "quoll/core/Base.h"
#include "LuaScriptInputVariable.h"

namespace quoll {

LuaScriptInputVariable::LuaScriptInputVariable(String value)
    : mValue(value), mType(LuaScriptVariableType::String) {}

LuaScriptInputVariable::LuaScriptInputVariable(AssetHandle<PrefabAsset> value)
    : mValue(value), mType(LuaScriptVariableType::AssetPrefab) {}

LuaScriptInputVariable::LuaScriptInputVariable(AssetHandle<TextureAsset> value)
    : mValue(value), mType(LuaScriptVariableType::AssetTexture) {}

} // namespace quoll

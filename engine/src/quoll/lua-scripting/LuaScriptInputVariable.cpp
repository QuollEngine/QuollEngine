#include "quoll/core/Base.h"
#include "LuaScriptInputVariable.h"

namespace quoll {

LuaScriptInputVariable::LuaScriptInputVariable(String value)
    : mValue(value), mType(LuaScriptVariableType::String) {}

LuaScriptInputVariable::LuaScriptInputVariable(AssetRef<PrefabAsset> value)
    : mValue(value), mType(LuaScriptVariableType::AssetPrefab) {}

LuaScriptInputVariable::LuaScriptInputVariable(AssetRef<TextureAsset> value)
    : mValue(value), mType(LuaScriptVariableType::AssetTexture) {}

} // namespace quoll

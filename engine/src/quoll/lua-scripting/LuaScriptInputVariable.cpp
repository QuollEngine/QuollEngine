#include "quoll/core/Base.h"
#include "LuaScriptInputVariable.h"

namespace quoll {

LuaScriptInputVariable::LuaScriptInputVariable(String value)
    : mValue(value), mType(LuaScriptVariableType::String) {}

LuaScriptInputVariable::LuaScriptInputVariable(PrefabAssetHandle value)
    : mValue(value), mType(LuaScriptVariableType::AssetPrefab) {}

LuaScriptInputVariable::LuaScriptInputVariable(TextureAssetHandle value)
    : mValue(value), mType(LuaScriptVariableType::AssetTexture) {}

} // namespace quoll

#include "liquid/core/Base.h"
#include "LuaScriptInputVariable.h"

namespace liquid {

LuaScriptInputVariable::LuaScriptInputVariable(String value)
    : mValue(value), mType(LuaScriptVariableType::String) {}

LuaScriptInputVariable::LuaScriptInputVariable(PrefabAssetHandle value)
    : mValue(value), mType(LuaScriptVariableType::AssetPrefab) {}

} // namespace liquid

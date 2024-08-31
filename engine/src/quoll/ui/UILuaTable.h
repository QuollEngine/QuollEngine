#pragma once

#include "quoll/lua-scripting/LuaHeaders.h"
#include "UIComponents.h"

namespace quoll {

class AssetCache;

class UILuaTable {
public:
  static sol::table create(sol::state_view state, AssetCache &assetCache);
};

} // namespace quoll

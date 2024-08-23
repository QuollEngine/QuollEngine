#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "InputMap.h"

namespace quoll {

struct InputMapSystemView {
  flecs::query<InputMapAssetRef> queryAssets;
  flecs::query<InputMap> queryInputMaps;
  flecs::query<> queryInputMapsWithoutAssets;
};

} // namespace quoll
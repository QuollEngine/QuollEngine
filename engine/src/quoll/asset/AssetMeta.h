#pragma once

#include "AssetType.h"

namespace quoll {

struct AssetMeta {
  AssetType type;

  String name;

  Uuid uuid;
};

} // namespace quoll

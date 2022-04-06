#pragma once

#include "Asset.h"

namespace liquid {

struct AssetFileHeader {
  const char magic[11] = "LASSETFILE";
  uint32_t version = 0;
  AssetType type = AssetType::None;
};

} // namespace liquid

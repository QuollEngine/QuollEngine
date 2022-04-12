#pragma once

#include "Asset.h"

namespace liquid {

constexpr size_t ASSET_FILE_MAGIC_LENGTH = 11;

struct AssetFileHeader {
  const char *magic = "LQASSETFILE";
  uint64_t version = 0;
  AssetType type = AssetType::None;
};

} // namespace liquid

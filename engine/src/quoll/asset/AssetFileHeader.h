#pragma once

#include "Asset.h"

namespace quoll {

static constexpr usize AssetFileMagicLength = 11;

struct AssetFileHeader {
  static constexpr const char *MagicConstant = "QLASSETFILE";

  String magic;

  String name;

  AssetType type = AssetType::None;
};

} // namespace quoll

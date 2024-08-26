#pragma once

#include "AssetType.h"

namespace quoll {

static constexpr usize AssetFileMagicLength = 11;

struct AssetFileHeader {
  static constexpr const char *MagicConstant = "QLASSETFILE";

  String magic;

  AssetType type = AssetType::None;
};

} // namespace quoll

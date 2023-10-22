#pragma once

#include "Asset.h"

namespace quoll {

static constexpr usize AssetFileMagicLength = 11;

/**
 * @brief Asset file header
 *
 * Used as a header for binary files
 */
struct AssetFileHeader {
  /**
   * Magic constant
   */
  static constexpr const char *MagicConstant = "QLASSETFILE";

  /**
   * Magic value
   */
  String magic;

  /**
   * Asset name
   */
  String name;

  /**
   * Asset type
   */
  AssetType type = AssetType::None;
};

} // namespace quoll

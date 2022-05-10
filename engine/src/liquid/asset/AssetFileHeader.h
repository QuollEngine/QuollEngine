#pragma once

#include "Asset.h"

namespace liquid {

constexpr size_t ASSET_FILE_MAGIC_LENGTH = 11;

/**
 * @brief Asset file header
 *
 * Used as a header for binary files
 */
struct AssetFileHeader {
  /**
   * Magic to identify the file
   */
  const char *magic = "LQASSETFILE";

  /**
   * Asset version
   */
  uint64_t version = 0;

  /**
   * Asset type
   */
  AssetType type = AssetType::None;
};

} // namespace liquid

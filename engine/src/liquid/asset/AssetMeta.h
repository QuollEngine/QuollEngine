#pragma once

namespace quoll {

/**
 * @brief Metadata for non liquid assets
 */
struct AssetMeta {
  /**
   * Asset type
   */
  AssetType type;

  /**
   * Asset name
   */
  String name;
};

} // namespace quoll

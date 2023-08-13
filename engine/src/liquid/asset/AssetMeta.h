#pragma once

namespace liquid {

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

} // namespace liquid

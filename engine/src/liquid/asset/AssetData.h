#pragma once

#include "Asset.h"

namespace liquid {

/**
 * @brief Asset data wrapper
 *
 * @tparam TData Asset data type
 */
template <class TData> struct AssetData {
  /**
   * Asset type
   */
  AssetType type = AssetType::None;

  /**
   * Asset name
   */
  String name;

  /**
   * Asset size
   */
  size_t size = 0;

  /**
   * Asset data
   */
  TData data;

  /**
   * Asset path
   */
  Path path;

  /**
   * Asset relative path
   */
  Path relativePath;
};

} // namespace liquid

#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "Asset.h"

namespace quoll {

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
   * Asset uuid
   */
  Uuid uuid;

  /**
   * Asset name
   */
  String name;

  /**
   * Preview texture
   */
  rhi::TextureHandle preview = rhi::TextureHandle::Null;
};

} // namespace quoll

#pragma once

#include "RenderHandle.h"

namespace quoll::rhi {

/**
 * @brief Texture view description
 */
struct TextureViewDescription {
  /**
   * Texture
   */
  TextureHandle texture = TextureHandle::Null;

  /**
   * Base mip level
   */
  u32 baseMipLevel = 0;

  /**
   * Number of levels
   */
  u32 mipLevelCount = 1;

  /**
   * Base layer
   */
  u32 baseLayer = 0;

  /**
   * Layer count
   */
  u32 layerCount = 1;

  /**
   * Debug name
   */
  String debugName;
};

} // namespace quoll::rhi

#pragma once

#include "RenderHandle.h"

namespace liquid::rhi {

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
  uint32_t baseMipLevel = 0;

  /**
   * Number of levels
   */
  uint32_t mipLevelCount = 1;

  /**
   * Base layer
   */
  uint32_t baseLayer = 0;

  /**
   * Layer count
   */
  uint32_t layerCount = 1;

  /**
   * Debug name
   */
  String debugName;
};

} // namespace liquid::rhi

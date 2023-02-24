#pragma once

namespace liquid::rhi {

/**
 * @brief Texture view description
 */
struct TextureViewDescription {
  /**
   * Texture
   */
  TextureHandle texture = TextureHandle::Invalid;

  /**
   * Base mip level
   */
  uint32_t level = 0;

  /**
   * Number of levels
   */
  uint32_t levelCount = 1;

  /**
   * Base layer
   */
  uint32_t layer = 0;

  /**
   * Layer count
   */
  uint32_t layerCount = 1;
};

} // namespace liquid::rhi

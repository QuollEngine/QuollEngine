#pragma once

namespace quoll::rhi {

/**
 * @brief Blit region
 */
struct BlitRegion {
  /**
   * Source mip level
   */
  uint32_t srcMipLevel = 0;

  /**
   * Source array layer start
   */
  uint32_t srcBaseArrayLayer = 0;

  /**
   * Source array layer count
   */
  uint32_t srcLayerCount = 0;

  /**
   * Source offsets
   */
  std::array<glm::ivec3, 2> srcOffsets;

  /**
   * Destination mip level
   */
  uint32_t dstMipLevel = 0;

  /**
   * Destination array layer start
   */
  uint32_t dstBaseArrayLayer = 0;

  /**
   * Destination array layer count
   */
  uint32_t dstLayerCount = 0;

  /**
   * Destination offsets
   */
  std::array<glm::ivec3, 2> dstOffsets;
};

} // namespace quoll::rhi

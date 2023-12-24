#pragma once

namespace quoll::rhi {

/**
 * @brief Blit region
 */
struct BlitRegion {
  /**
   * Source mip level
   */
  u32 srcMipLevel = 0;

  /**
   * Source array layer start
   */
  u32 srcBaseArrayLayer = 0;

  /**
   * Source array layer count
   */
  u32 srcLayerCount = 0;

  /**
   * Source offsets
   */
  std::array<glm::ivec3, 2> srcOffsets;

  /**
   * Destination mip level
   */
  u32 dstMipLevel = 0;

  /**
   * Destination array layer start
   */
  u32 dstBaseArrayLayer = 0;

  /**
   * Destination array layer count
   */
  u32 dstLayerCount = 0;

  /**
   * Destination offsets
   */
  std::array<glm::ivec3, 2> dstOffsets;
};

} // namespace quoll::rhi

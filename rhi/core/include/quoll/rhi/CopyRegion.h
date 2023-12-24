#pragma once

namespace quoll::rhi {

/**
 * @brief Copy region
 */
struct CopyRegion {
  /**
   * Offset of buffer during copy
   */
  u32 bufferOffset = 0;

  /**
   * Image array layer start
   */
  u32 imageBaseArrayLayer = 0;

  /**
   * Image array layer count
   */
  u32 imageLayerCount = 1;

  /**
   * Image mip level
   */
  u32 imageLevel = 0;

  /**
   * Image offset
   */
  glm::ivec3 imageOffset{0};

  /**
   * Image extent
   */
  glm::uvec3 imageExtent{0};
};

} // namespace quoll::rhi

#pragma once

namespace liquid::rhi {

/**
 * @brief Copy region
 */
struct CopyRegion {
  /**
   * Offset of buffer during copy
   */
  uint32_t bufferOffset = 0;

  /**
   * Image array layer start
   */
  uint32_t imageBaseArrayLayer = 0;

  /**
   * Image array layer count
   */
  uint32_t imageLayerCount = 1;

  /**
   * Image mip level
   */
  uint32_t imageLevel = 0;

  /**
   * Image offset
   */
  glm::ivec3 imageOffset{0};

  /**
   * Image extent
   */
  glm::uvec3 imageExtent{0};
};

} // namespace liquid::rhi

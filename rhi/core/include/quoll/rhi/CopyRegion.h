#pragma once

namespace quoll::rhi {

struct CopyRegion {
  u32 bufferOffset = 0;

  u32 imageBaseArrayLayer = 0;

  u32 imageLayerCount = 1;

  u32 imageLevel = 0;

  glm::ivec3 imageOffset{0};

  glm::uvec3 imageExtent{0};
};

} // namespace quoll::rhi

#pragma once

namespace quoll::rhi {

struct BlitRegion {
  u32 srcMipLevel = 0;

  u32 srcBaseArrayLayer = 0;

  u32 srcLayerCount = 0;

  std::array<glm::ivec3, 2> srcOffsets;

  u32 dstMipLevel = 0;

  u32 dstBaseArrayLayer = 0;

  u32 dstLayerCount = 0;

  std::array<glm::ivec3, 2> dstOffsets;
};

} // namespace quoll::rhi

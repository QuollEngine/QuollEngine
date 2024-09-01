#include "quoll/core/Base.h"
#include "MeshRenderUtils.h"

namespace quoll {

static constexpr usize PositionsIndex = 0;
static constexpr usize JointsIndex = 5;
static constexpr usize WeightsIndex = 6;

std::array<rhi::BufferHandle, 1>
MeshRenderUtils::getGeometryBuffers(const MeshDrawData *drawData) {
  return std::array{drawData->vertexBuffers.at(PositionsIndex)};
}

std::array<u64, 1>
MeshRenderUtils::getGeometryBufferOffsets(const MeshDrawData *drawData) {
  return std::array{drawData->vertexBufferOffsets.at(PositionsIndex)};
}

std::array<rhi::BufferHandle, MeshRenderUtils::SkinGeometryContributors>
MeshRenderUtils::getSkinnedGeometryBuffers(const MeshDrawData *drawData) {
  return std::array{drawData->vertexBuffers.at(PositionsIndex),
                    drawData->vertexBuffers.at(JointsIndex),
                    drawData->vertexBuffers.at(WeightsIndex)};
}

std::array<u64, MeshRenderUtils::SkinGeometryContributors>
MeshRenderUtils::getSkinnedGeometryBufferOffsets(const MeshDrawData *drawData) {
  return std::array{drawData->vertexBufferOffsets.at(PositionsIndex),
                    drawData->vertexBufferOffsets.at(JointsIndex),
                    drawData->vertexBufferOffsets.at(WeightsIndex)};
}

} // namespace quoll

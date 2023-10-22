#include "quoll/core/Base.h"
#include "MeshRenderUtils.h"

namespace quoll {

static constexpr usize PositionsIndex = 0;
static constexpr usize JointsIndex = 5;
static constexpr usize WeightsIndex = 6;

std::array<rhi::BufferHandle, 1>
MeshRenderUtils::getGeometryBuffers(const MeshAsset &mesh) {

  return std::array{mesh.vertexBuffers.at(PositionsIndex)};
}

std::array<u64, 1>
MeshRenderUtils::getGeometryBufferOffsets(const MeshAsset &mesh) {
  return std::array{mesh.vertexBufferOffsets.at(PositionsIndex)};
}

std::array<rhi::BufferHandle, MeshRenderUtils::SkinGeometryContributors>
MeshRenderUtils::getSkinnedGeometryBuffers(const MeshAsset &mesh) {
  return std::array{mesh.vertexBuffers.at(PositionsIndex),
                    mesh.vertexBuffers.at(JointsIndex),
                    mesh.vertexBuffers.at(WeightsIndex)};
}

std::array<u64, MeshRenderUtils::SkinGeometryContributors>
MeshRenderUtils::getSkinnedGeometryBufferOffsets(const MeshAsset &mesh) {
  return std::array{mesh.vertexBufferOffsets.at(PositionsIndex),
                    mesh.vertexBufferOffsets.at(JointsIndex),
                    mesh.vertexBufferOffsets.at(WeightsIndex)};
}

} // namespace quoll

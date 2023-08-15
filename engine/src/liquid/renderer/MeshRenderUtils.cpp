#include "liquid/core/Base.h"
#include "MeshRenderUtils.h"

namespace liquid {

static constexpr size_t PositionsIndex = 0;
static constexpr size_t JointsIndex = 5;
static constexpr size_t WeightsIndex = 6;

std::array<rhi::BufferHandle, 1>
MeshRenderUtils::getGeometryBuffers(const MeshAsset &mesh) {

  return std::array{mesh.vertexBuffers.at(PositionsIndex)};
}

std::array<uint64_t, 1>
MeshRenderUtils::getGeometryBufferOffsets(const MeshAsset &mesh) {
  return std::array{mesh.vertexBufferOffsets.at(PositionsIndex)};
}

std::array<rhi::BufferHandle, MeshRenderUtils::SkinGeometryContributors>
MeshRenderUtils::getSkinnedGeometryBuffers(const MeshAsset &mesh) {
  return std::array{mesh.vertexBuffers.at(PositionsIndex),
                    mesh.vertexBuffers.at(JointsIndex),
                    mesh.vertexBuffers.at(WeightsIndex)};
}

std::array<uint64_t, MeshRenderUtils::SkinGeometryContributors>
MeshRenderUtils::getSkinnedGeometryBufferOffsets(const MeshAsset &mesh) {
  return std::array{mesh.vertexBufferOffsets.at(PositionsIndex),
                    mesh.vertexBufferOffsets.at(JointsIndex),
                    mesh.vertexBufferOffsets.at(WeightsIndex)};
}

} // namespace liquid

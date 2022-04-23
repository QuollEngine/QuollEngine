#pragma once

#include "liquid/scene/Vertex.h"
#include "liquid/scene/SkinnedVertex.h"
#include "liquid/rhi/RenderHandle.h"

#include "Asset.h"

namespace liquid {

template <class TVertex> struct BaseGeometryAsset {
  std::vector<TVertex> vertices;
  std::vector<uint32_t> indices;
  MaterialAssetHandle material = MaterialAssetHandle::Invalid;
};

struct MeshAsset {
  std::vector<BaseGeometryAsset<Vertex>> geometries;
  std::vector<rhi::BufferHandle> vertexBuffers;
  std::vector<rhi::BufferHandle> indexBuffers;
};

struct SkinnedMeshAsset {
  std::vector<BaseGeometryAsset<SkinnedVertex>> geometries;
  SkeletonAssetHandle skeleton = SkeletonAssetHandle::Invalid;
  std::vector<rhi::BufferHandle> vertexBuffers;
  std::vector<rhi::BufferHandle> indexBuffers;
};

} // namespace liquid

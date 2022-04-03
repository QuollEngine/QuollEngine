#pragma once

#include "liquid/scene/Vertex.h"
#include "liquid/scene/SkinnedVertex.h"

namespace liquid {

template <class TVertex> struct BaseGeometryAsset {
  std::vector<TVertex> vertices;
  std::vector<uint32_t> indices;
  MaterialAssetHandle material = MaterialAssetHandle::Invalid;
};

struct MeshAsset {
  std::vector<BaseGeometryAsset<Vertex>> geometries;
};

struct SkinnedMeshAsset {
  std::vector<BaseGeometryAsset<SkinnedVertex>> geometries;
  SkeletonAssetHandle skeleton = SkeletonAssetHandle::Invalid;
};

} // namespace liquid

#pragma once

namespace liquid {

template <class TVertex> struct MeshAsset {
  struct Geometry {
    std::vector<TVertex> vertices;
    std::vector<uint32_t> indices;
    MaterialAssetHandle material = MaterialAssetHandle::Invalid;
  };

  std::vector<Geometry> geometries;
};

} // namespace liquid

#pragma once

#include "liquid/scene/Vertex.h"
#include "liquid/scene/SkinnedVertex.h"
#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/Buffer.h"

#include "Asset.h"

namespace liquid {

class Material;

/**
 * @brief Base geometry asset data
 *
 * @tparam TVertex Vertex
 */
template <class TVertex> struct BaseGeometryAsset {
  /**
   * List of vertices
   */
  std::vector<TVertex> vertices;

  /**
   * List of indices
   */
  std::vector<uint32_t> indices;

  /**
   * Material handle
   */
  MaterialAssetHandle material = MaterialAssetHandle::Invalid;
};

/**
 * @brief Mesh asset data
 */
struct MeshAsset {
  /**
   * List of geometries
   */
  std::vector<BaseGeometryAsset<Vertex>> geometries;

  /**
   * List of vertex buffers
   */
  std::vector<rhi::Buffer> vertexBuffers;

  /**
   * List of index buffers
   */
  std::vector<rhi::Buffer> indexBuffers;

  /**
   * List of materials
   */
  std::vector<SharedPtr<Material>> materials;
};

/**
 * @brief Skinned mesh asset data
 */
struct SkinnedMeshAsset {
  /**
   * List of geometries
   */
  std::vector<BaseGeometryAsset<SkinnedVertex>> geometries;

  /**
   * Skeleton
   */
  SkeletonAssetHandle skeleton = SkeletonAssetHandle::Invalid;
  /**
   * List of vertex buffers
   */
  std::vector<rhi::Buffer> vertexBuffers;

  /**
   * List of index buffers
   */
  std::vector<rhi::Buffer> indexBuffers;

  /**
   * List of materials
   */
  std::vector<SharedPtr<Material>> materials;
};

} // namespace liquid

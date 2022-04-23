#pragma once

#include "liquid/renderer/Material.h"
#include "liquid/rhi/ResourceRegistry.h"
#include "liquid/asset/MeshAsset.h"
#include "liquid/asset/AssetData.h"

namespace liquid {

class MeshInstance {
public:
  template <class TMeshAsset>
  MeshInstance(const AssetData<TMeshAsset> &asset,
               const std::vector<SharedPtr<Material>> &materials) {

    LIQUID_ASSERT(asset.data.vertexBuffers.size() ==
                      asset.data.indexBuffers.size(),
                  "Vertex and index buffer sizes must match");
    LIQUID_ASSERT(asset.data.vertexBuffers.size() == materials.size(),
                  "Materials size must match buffer sizes");

    mVertexBuffers = asset.data.vertexBuffers;
    mIndexBuffers = asset.data.indexBuffers;
    mMaterials = materials;

    mVertexCounts.resize(asset.data.geometries.size());
    mIndexCounts.resize(asset.data.geometries.size());
    for (size_t i = 0; i < asset.data.geometries.size(); ++i) {
      const auto &geometry = asset.data.geometries.at(i);
      mVertexCounts.at(i) = static_cast<uint32_t>(geometry.vertices.size());
      mIndexCounts.at(i) = static_cast<uint32_t>(geometry.indices.size());
    }
  }

  /**
   * @brief Get vertex buffers
   *
   * @return Vertex buffers
   */
  inline const std::vector<rhi::BufferHandle> &getVertexBuffers() const {
    return mVertexBuffers;
  }

  /**
   * @brief Get index buffers
   *
   * @return Index buffers
   */
  inline const std::vector<rhi::BufferHandle> &getIndexBuffers() const {
    return mIndexBuffers;
  }

  /**
   * @brief Get materials
   *
   * @return List of materials
   */
  inline const std::vector<SharedPtr<Material>> &getMaterials() const {
    return mMaterials;
  }

  /**
   * @brief Get vertex counts
   *
   * @return Vertex counts
   */
  inline const std::vector<uint32_t> &getVertexCounts() const {
    return mVertexCounts;
  }

  /**
   * @brief Get index counts
   *
   * @return Index counts
   */
  inline const std::vector<uint32_t> &getIndexCounts() const {
    return mIndexCounts;
  }

private:
  std::vector<rhi::BufferHandle> mVertexBuffers;
  std::vector<rhi::BufferHandle> mIndexBuffers;
  std::vector<SharedPtr<Material>> mMaterials;
  std::vector<uint32_t> mVertexCounts;
  std::vector<uint32_t> mIndexCounts;
};

} // namespace liquid

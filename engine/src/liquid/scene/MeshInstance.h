#pragma once

#include "liquid/renderer/Material.h"

#include "liquid/rhi/ResourceRegistry.h"

namespace liquid {

template <class Mesh> class MeshInstance {
public:
  /**
   * @brief Creates mesh instance from mesh
   *
   * Uses resource allocator to create
   * hardware buffers from mesh data
   *
   * @param mesh Mesh
   * @param registry Resource registry
   */
  MeshInstance(const Mesh &mesh, rhi::ResourceRegistry &registry)
      : mRegistry(registry), mMesh(mesh) {
    for (auto &geometry : mMesh.getGeometries()) {
      size_t bufferSize =
          geometry.getVertices().size() * sizeof(typename Mesh::Vertex);

      auto vertexBuffer =
          registry.setBuffer({rhi::BufferType::Vertex, bufferSize,
                              (void *)geometry.getVertices().data()});

      if (geometry.getIndices().size() > 0) {
        size_t bufferSize = geometry.getIndices().size() * sizeof(uint32_t);

        auto indexBuffer =
            registry.setBuffer({rhi::BufferType::Index,
                                geometry.getIndices().size() * sizeof(uint32_t),
                                (void *)geometry.getIndices().data()});
        mIndexBuffers.push_back(indexBuffer);
      } else {
        mIndexBuffers.push_back(rhi::BufferHandle::Invalid);
      }

      mIndexCounts.push_back(
          static_cast<uint32_t>(geometry.getIndices().size()));
      mVertexCounts.push_back(
          static_cast<uint32_t>(geometry.getVertices().size()));
      mVertexBuffers.push_back(vertexBuffer);
      mMaterials.push_back(geometry.getMaterial());
    }
  }

  ~MeshInstance() {
    for (auto &x : mVertexBuffers) {
      mRegistry.deleteBuffer(x);
    }

    for (auto &x : mIndexBuffers) {
      mRegistry.deleteBuffer(x);
    }
  }

  MeshInstance(const MeshInstance &rhs) = delete;
  MeshInstance(MeshInstance &&rhs) = delete;
  MeshInstance &operator=(const MeshInstance &rhs) = delete;
  MeshInstance &operator=(MeshInstance &&rhs) = delete;

  /**
   * @brief Set material
   *
   * @param material Material
   * @param index Index
   */
  void setMaterial(const SharedPtr<Material> &material, size_t index = 0) {
    mMaterials.at(index) = material;
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

  Mesh mMesh;
  rhi::ResourceRegistry &mRegistry;
};

} // namespace liquid

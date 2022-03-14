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
          registry.addBuffer({rhi::BufferType::Vertex, bufferSize,
                              (void *)geometry.getVertices().data()});

      if (geometry.getIndices().size() > 0) {
        size_t bufferSize = geometry.getIndices().size() * sizeof(uint32_t);

        auto indexBuffer =
            registry.addBuffer({rhi::BufferType::Index,
                                geometry.getIndices().size() * sizeof(uint32_t),
                                (void *)geometry.getIndices().data()});
        indexBuffers.push_back(indexBuffer);
      } else {
        indexBuffers.push_back(0);
      }

      indexCounts.push_back(
          static_cast<uint32_t>(geometry.getIndices().size()));
      vertexCounts.push_back(
          static_cast<uint32_t>(geometry.getVertices().size()));
      vertexBuffers.push_back(vertexBuffer);
      materials.push_back(geometry.getMaterial());
    }
  }

  ~MeshInstance() {
    for (auto &x : vertexBuffers) {
      mRegistry.deleteBuffer(x);
    }

    for (auto &x : indexBuffers) {
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
    materials.at(index) = material;
  }

  /**
   * @brief Get vertex buffers
   *
   * @return Vertex buffers
   */
  inline const std::vector<rhi::BufferHandle> &getVertexBuffers() const {
    return vertexBuffers;
  }

  /**
   * @brief Get index buffers
   *
   * @return Index buffers
   */
  inline const std::vector<rhi::BufferHandle> &getIndexBuffers() const {
    return indexBuffers;
  }

  /**
   * @brief Get materials
   *
   * @return List of materials
   */
  inline const std::vector<SharedPtr<Material>> &getMaterials() const {
    return materials;
  }

  /**
   * @brief Get vertex counts
   *
   * @return Vertex counts
   */
  inline const std::vector<uint32_t> &getVertexCounts() const {
    return vertexCounts;
  }

  /**
   * @brief Get index counts
   *
   * @return Index counts
   */
  inline const std::vector<uint32_t> &getIndexCounts() const {
    return indexCounts;
  }

private:
  std::vector<rhi::BufferHandle> vertexBuffers;
  std::vector<rhi::BufferHandle> indexBuffers;
  std::vector<SharedPtr<Material>> materials;
  std::vector<uint32_t> vertexCounts;
  std::vector<uint32_t> indexCounts;

  Mesh mMesh;
  rhi::ResourceRegistry &mRegistry;
};

} // namespace liquid

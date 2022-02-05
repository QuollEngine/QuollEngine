#pragma once

#include "liquid/renderer/HardwareBuffer.h"
#include "liquid/renderer/ResourceAllocator.h"
#include "liquid/renderer/Material.h"

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
   * @param resourceAllocator Resource allocator
   */
  MeshInstance(Mesh *mesh, ResourceAllocator *resourceAllocator) {
    for (auto &geometry : mesh->getGeometries()) {
      const auto &vertexBuffer = resourceAllocator->createVertexBuffer(
          geometry.getVertices().size() * sizeof(typename Mesh::Vertex));

      vertexBuffer->update((void *)geometry.getVertices().data());

      if (geometry.getIndices().size() > 0) {
        const auto &indexBuffer = resourceAllocator->createIndexBuffer(
            geometry.getIndices().size() * sizeof(uint32_t));

        indexBuffer->update((void *)geometry.getIndices().data());
        indexBuffers.push_back(indexBuffer);
      } else {
        indexBuffers.push_back(nullptr);
      }

      indexCounts.push_back(geometry.getIndices().size());
      vertexCounts.push_back(geometry.getVertices().size());
      vertexBuffers.push_back(vertexBuffer);
      materials.push_back(geometry.getMaterial());
    }
  }

  ~MeshInstance() = default;
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
  inline const std::vector<SharedPtr<HardwareBuffer>> &
  getVertexBuffers() const {
    return vertexBuffers;
  }

  /**
   * @brief Get index buffers
   *
   * @return Index buffers
   */
  inline const std::vector<SharedPtr<HardwareBuffer>> &getIndexBuffers() const {
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
  inline const std::vector<size_t> &getVertexCounts() const {
    return vertexCounts;
  }

  /**
   * @brief Get index counts
   *
   * @return Index counts
   */
  inline const std::vector<size_t> &getIndexCounts() const {
    return indexCounts;
  }

private:
  std::vector<SharedPtr<HardwareBuffer>> vertexBuffers;
  std::vector<SharedPtr<HardwareBuffer>> indexBuffers;
  std::vector<SharedPtr<Material>> materials;
  std::vector<size_t> vertexCounts;
  std::vector<size_t> indexCounts;
};

} // namespace liquid

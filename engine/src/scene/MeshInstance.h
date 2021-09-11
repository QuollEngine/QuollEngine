#pragma once

#include "Mesh.h"
#include "renderer/HardwareBuffer.h"
#include "renderer/ResourceAllocator.h"
#include "renderer/Material.h"

namespace liquid {

class MeshInstance {
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
  MeshInstance(Mesh *mesh, ResourceAllocator *resourceAllocator);

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
  void setMaterial(const SharedPtr<Material> &material, size_t index = 0);

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
  std::vector<size_t> indexCounts;
};

} // namespace liquid

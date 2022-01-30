#include "liquid/core/Base.h"
#include "MeshInstance.h"

namespace liquid {

MeshInstance::MeshInstance(Mesh *mesh, ResourceAllocator *resourceAllocator) {
  for (auto &geometry : mesh->getGeometries()) {
    const auto &vertexBuffer = resourceAllocator->createVertexBuffer(
        geometry.getVertices().size() * sizeof(Vertex));

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

void MeshInstance::setMaterial(const SharedPtr<Material> &material,
                               size_t index) {
  materials.at(index) = material;
}

} // namespace liquid

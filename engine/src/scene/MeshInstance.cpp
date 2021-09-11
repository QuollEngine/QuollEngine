#include "MeshInstance.h"

namespace liquid {

MeshInstance::MeshInstance(Mesh *mesh, ResourceAllocator *resourceAllocator) {
  for (auto &geometry : mesh->getGeometries()) {
    const auto &vertexBuffer = resourceAllocator->createVertexBuffer(
        geometry.getVertices().size() * sizeof(Vertex));
    const auto &indexBuffer = resourceAllocator->createIndexBuffer(
        geometry.getIndices().size() * sizeof(uint32_t));

    vertexBuffer->update((void *)geometry.getVertices().data());
    indexBuffer->update((void *)geometry.getIndices().data());

    vertexBuffers.push_back(vertexBuffer);
    indexBuffers.push_back(indexBuffer);
    materials.push_back(geometry.getMaterial());
    indexCounts.push_back(geometry.getIndices().size());
  }
}

void MeshInstance::setMaterial(const SharedPtr<Material> &material,
                               size_t index) {
  materials.at(index) = material;
}

} // namespace liquid

#include "MeshInstance.h"

namespace liquid {

MeshInstance::MeshInstance(Mesh *mesh, ResourceAllocator *resourceAllocator) {
  for (auto &geometry : mesh->getGeometries()) {
    vertexBuffers.push_back(
        resourceAllocator->createVertexBuffer(geometry.getVertices()));
    indexBuffers.push_back(
        resourceAllocator->createIndexBuffer(geometry.getIndices()));
    materials.push_back(geometry.getMaterial());
  }
}

MeshInstance::~MeshInstance() {
  for (auto &x : vertexBuffers) {
    delete x;
  }

  vertexBuffers.clear();

  for (auto &x : indexBuffers) {
    delete x;
  }

  indexBuffers.clear();

  materials.clear();
}

void MeshInstance::setMaterial(const SharedPtr<Material> &material_,
                               size_t index) {
  materials.at(index) = material_;
}

} // namespace liquid

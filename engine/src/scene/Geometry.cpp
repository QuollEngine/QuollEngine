#include "Geometry.h"

namespace liquid {

Geometry::Geometry(const std::vector<Vertex> &vertices_,
                   const std::vector<uint32_t> &indices_,
                   const SharedPtr<Material> &material_)
    : vertices(vertices_), indices(indices_), material(material_) {}

void Geometry::addVertex(const Vertex &vertex) { vertices.push_back(vertex); }

void Geometry::addTriangle(uint32_t p1, uint32_t p2, uint32_t p3) {
  indices.push_back(p1);
  indices.push_back(p2);
  indices.push_back(p3);
}

void Geometry::setMaterial(const SharedPtr<Material> &material_) {
  material = material_;
}

} // namespace liquid

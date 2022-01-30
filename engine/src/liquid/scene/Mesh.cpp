#include "liquid/core/Base.h"
#include "Mesh.h"

namespace liquid {

void Mesh::addGeometry(const Geometry &geometry) {
  geometries.push_back(geometry);
}

} // namespace liquid

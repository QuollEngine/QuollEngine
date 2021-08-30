#pragma once

#include "core/Base.h"
#include "Vertex.h"
#include "Geometry.h"

namespace liquid {

class Mesh {
public:
  /**
   * @brief Default Constructor
   */
  Mesh() = default;

  /**
   * @brief Add geometry
   *
   * @param geometry Geometry
   */
  void addGeometry(const Geometry &geometry);

  /**
   * @brief Get geometries
   *
   * @return List of geometries
   */
  inline const std::vector<Geometry> &getGeometries() const {
    return geometries;
  }

private:
  std::vector<Geometry> geometries;
};

} // namespace liquid

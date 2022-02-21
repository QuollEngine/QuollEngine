#pragma once

#include "BaseGeometry.h"

namespace liquid {

/**
 * @brief Base mesh
 *
 * @tparam TVertex Vertex type
 */
template <class TVertex> class BaseMesh {
public:
  using Vertex = TVertex;
  using Geometry = BaseGeometry<TVertex>;

public:
  /**
   * @brief Add geometry
   *
   * @param geometry Geometry
   */
  void addGeometry(const Geometry &geometry) { geometries.push_back(geometry); }

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

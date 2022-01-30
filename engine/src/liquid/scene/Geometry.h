
#pragma once

#include "liquid/core/Base.h"
#include "Vertex.h"
#include "liquid/renderer/Material.h"

namespace liquid {

class Geometry {
public:
  /**
   * @brief Default Constructor
   */
  Geometry() = default;

  /**
   * @brief Create geometry from vertices and indices
   *
   * @param vertices List of vertices
   * @param indices List of indices
   * @param material Material
   */
  Geometry(const std::vector<Vertex> &vertices,
           const std::vector<uint32_t> &indices,
           const SharedPtr<Material> &material);

  /**
   * @brief Add vertex
   *
   * @param vertex Vertex
   */
  void addVertex(const Vertex &vertex);

  /**
   * @brief Add triangle
   *
   * @param p1 Vertex index of first point
   * @param p2 Vertex index of second point
   * @param p3 Vertex index of third point
   */
  void addTriangle(uint32_t p1, uint32_t p2, uint32_t p3);

  /**
   * @brief Set material
   *
   * @param material
   */
  void setMaterial(const SharedPtr<Material> &material);

  /**
   * @brief Get vertices
   *
   * @return List of vertices
   */
  inline const std::vector<Vertex> &getVertices() const { return vertices; }

  /**
   * @brief Get indices
   *
   * @return List of indices
   */
  inline const std::vector<uint32_t> &getIndices() const { return indices; }

  /**
   * @brief Get material
   *
   * @return Material
   */
  inline const SharedPtr<Material> &getMaterial() const { return material; }

private:
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  SharedPtr<Material> material;
};

} // namespace liquid

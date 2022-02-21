
#pragma once

#include "liquid/renderer/Material.h"

namespace liquid {

/**
 * @brief Base geometry
 *
 * @tparam TVertex Vertex type
 */
template <class TVertex> class BaseGeometry {
public:
  using Vertex = TVertex;

public:
  /**
   * @brief Default Constructor
   */
  BaseGeometry() = default;

  /**
   * @brief Create geometry from vertices and indices
   *
   * @param vertices List of vertices
   * @param indices List of indices
   * @param material Material
   */
  BaseGeometry(const std::vector<Vertex> &vertices_,
               const std::vector<uint32_t> &indices_,
               const SharedPtr<Material> &material_)
      : vertices(vertices_), indices(indices_), material(material_) {}

  /**
   * @brief Add vertex
   *
   * @param vertex Vertex
   */
  void addVertex(const Vertex &vertex) { vertices.push_back(vertex); }

  /**
   * @brief Add triangle
   *
   * @param p1 Vertex index of first point
   * @param p2 Vertex index of second point
   * @param p3 Vertex index of third point
   */
  void addTriangle(uint32_t p1, uint32_t p2, uint32_t p3) {
    indices.push_back(p1);
    indices.push_back(p2);
    indices.push_back(p3);
  }

  /**
   * @brief Set material
   *
   * @param material Material
   */
  void setMaterial(const SharedPtr<Material> &material_) {
    material = material_;
  }

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

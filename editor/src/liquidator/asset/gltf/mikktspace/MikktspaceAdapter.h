#pragma once

#include "mikktspace.h"

namespace quoll::editor {

static constexpr uint32_t TriangleVertices = 3;

/**
 * @brief Mikktspace adapter
 */
class MikktspaceAdapter {
  struct MeshData {
    const std::vector<glm::vec3> &positions;
    const std::vector<glm::vec3> &normals;
    const std::vector<glm::vec2> &texCoords;
    const std::vector<uint32_t> &indices;
    std::vector<glm::vec4> &tangents;
  };

public:
  /**
   * @brief Mikktspace adapter
   */
  MikktspaceAdapter();

  /**
   * @brief Generate tangents
   *
   * @param positions Vertex positions
   * @param normals Vertex normals
   * @param texCoords Vertex texture coordinates
   * @param indices Indices
   * @param[out] tangents Output tangents
   */
  void generate(const std::vector<glm::vec3> &positions,
                const std::vector<glm::vec3> &normals,
                const std::vector<glm::vec2> &texCoords,
                const std::vector<uint32_t> &indices,
                std::vector<glm::vec4> &tangents);

private:
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)

  /**
   * @brief Get number of faces
   *
   * @param context Mikktspace context
   * @return Number of triangles based on indices
   */
  static int getNumFaces(const SMikkTSpaceContext *pContext);

  /**
   * @brief Get number of vertices of a face
   *
   * @param context Mikktspace context
   * @param faceIndex Face index (unused)
   * @return Vertices of triangle
   */
  static inline int getNumVerticesOfFace(const SMikkTSpaceContext *pContext,
                                         const int faceIndex) {
    return TriangleVertices;
  }

  /**
   * @brief Get position
   *
   * @param context Mikktpsace context
   * @param outAttribute Output position
   * @param faceIndex Face index
   * @param vertexIndex Relative vertex index
   */
  static void getPosition(const SMikkTSpaceContext *context,
                          float outAttribute[], const int faceIndex,
                          const int vertexIndex);

  /**
   * @brief Get normal
   *
   * @param context Mikktpsace context
   * @param outAttribute Output normal
   * @param faceIndex Face index
   * @param vertexIndex Relative vertex index
   */
  static void getNormal(const SMikkTSpaceContext *context, float outAttribute[],
                        const int faceIndex, const int vertexIndex);

  /**
   * @brief Get texture coordinate
   *
   * @param context Mikktpsace context
   * @param outAttribute Output texture coordinate
   * @param faceIndex Face index
   * @param vertexIndex Relative vertex index
   */
  static void getTexCoord(const SMikkTSpaceContext *context,
                          float outAttribute[], const int faceIndex,
                          const int vertexIndex);

  /**
   * @brief Set tangent space
   *
   * @param context Mikktpsace context
   * @param tangents Tangent data
   * @param sign Tangent sign
   * @param faceIndex Face index
   * @param vertexIndex Relative vertex index
   */
  static void setTSpaceBasic(const SMikkTSpaceContext *context,
                             const float tangents[], const float sign,
                             const int faceIndex, const int vertexIndex);

  // NOLINTEND(cppcoreguidelines-avoid-c-arrays)
private:
  /**
   * @brief Get mesh data from context
   *
   * @param context Mikktpsace context
   * @return Mesh data
   */
  static inline MeshData &getMeshData(const SMikkTSpaceContext *context) {
    return *static_cast<MeshData *>(context->m_pUserData);
  }

  /**
   * @brief Get vertex index from face index and relative vertex index
   *
   * @param context Mikktpsace context
   * @param faceIndex Face index
   * @param vertexIndex Vertex index relative to face
   * @return Vertex index
   */
  static size_t getVertexIndex(const SMikkTSpaceContext *context, int faceIndex,
                               int vertexIndex);

private:
  SMikkTSpaceInterface mInterface{};
};

} // namespace quoll::editor

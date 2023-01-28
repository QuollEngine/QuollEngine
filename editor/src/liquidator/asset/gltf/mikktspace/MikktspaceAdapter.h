#pragma once

#include "mikktspace.h"

namespace liquid::editor {

static constexpr uint32_t TriangleVertices = 3;

/**
 * @brief Mikktspace adapter
 *
 * @tparam TVertex Vertex type
 */
template <class TVertex> class MikktspaceAdapter {
  /**
   * @brief Mesh data
   */
  struct MeshData {
    /**
     * @brief Vertices
     */
    std::vector<TVertex> &vertices;

    /**
     * @brief Indices
     */
    const std::vector<uint32_t> &indices;
  };

public:
  /**
   * @brief Mikktspace adapter
   */
  MikktspaceAdapter() {
    mInterface.m_getNumFaces = getNumFaces;
    mInterface.m_getNumVerticesOfFace = getNumVerticesOfFace;
    mInterface.m_getPosition = getPosition;
    mInterface.m_getNormal = getNormal;
    mInterface.m_getTexCoord = getTexCoord;
    mInterface.m_setTSpaceBasic = setTSpaceBasic;
    // We do not need the custom tangent
    // space generator since we are using
    // basic one.
    mInterface.m_setTSpace = nullptr;
  }

  /**
   * @brief Generate tangents
   *
   * @param vertices Vertices
   * @param indices Indices
   */
  void generate(std::vector<TVertex> &vertices,
                const std::vector<uint32_t> &indices) {
    MeshData data{vertices, indices};

    SMikkTSpaceContext context{};
    context.m_pInterface = &mInterface;
    context.m_pUserData = &data;

    genTangSpaceDefault(&context);
  }

private:
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
  /**
   * @brief Get number of faces
   *
   * @param context Mikktspace context
   * @return Number of triangles based on indices
   */
  static int getNumFaces(const SMikkTSpaceContext *pContext) {
    auto &data = getMeshData(pContext);

    return static_cast<int>(data.indices.size()) / TriangleVertices;
  }

  /**
   * @brief Get number of vertices of a face
   *
   * @param context Mikktspace context
   * @param faceIndex Face index (unused)
   * @return Vertices of triangle
   */
  static int getNumVerticesOfFace(const SMikkTSpaceContext *pContext,
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
                          const int vertexIndex) {
    auto &vertex = getVertex(context, faceIndex, vertexIndex);

    outAttribute[0] = vertex.x;
    outAttribute[1] = vertex.y;
    outAttribute[2] = vertex.z;
  }

  /**
   * @brief Get normal
   *
   * @param context Mikktpsace context
   * @param outAttribute Output normal
   * @param faceIndex Face index
   * @param vertexIndex Relative vertex index
   */
  static void getNormal(const SMikkTSpaceContext *context, float outAttribute[],
                        const int faceIndex, const int vertexIndex) {
    auto &vertex = getVertex(context, faceIndex, vertexIndex);

    outAttribute[0] = vertex.nx;
    outAttribute[1] = vertex.ny;
    outAttribute[2] = vertex.nz;
  }

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
                          const int vertexIndex) {
    auto &vertex = getVertex(context, faceIndex, vertexIndex);

    outAttribute[0] = vertex.u0;
    outAttribute[1] = vertex.v0;
  }

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
                             const int faceIndex, const int vertexIndex) {
    auto &vertex = getVertex(context, faceIndex, vertexIndex);

    vertex.tx = tangents[0];
    vertex.ty = tangents[1];
    vertex.tz = tangents[2];
    vertex.tw = sign;
  }
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays)

private:
  /**
   * @brief Get mesh data from context
   *
   * @param context Mikktpsace context
   * @return Mesh data
   */
  static MeshData &getMeshData(const SMikkTSpaceContext *context) {
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
                               int vertexIndex) {
    auto &meshData = getMeshData(context);
    size_t index = static_cast<size_t>(faceIndex * 3 + vertexIndex);

    return meshData.indices.at(index);
  }

  /**
   * @brief Get vertex from face index and relative vertex index
   *
   * @param context Mikktpsace context
   * @param faceIndex Face index
   * @param vertexIndex Vertex index relative to face
   * @return Vertex
   */
  static TVertex &getVertex(const SMikkTSpaceContext *context, int faceIndex,
                            int vertexIndex) {
    auto &meshData = getMeshData(context);
    auto index = getVertexIndex(context, faceIndex, vertexIndex);
    return meshData.vertices.at(index);
  }

private:
  SMikkTSpaceInterface mInterface{};
};

} // namespace liquid::editor

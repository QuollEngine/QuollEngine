#pragma once

#include "mikktspace.h"

namespace quoll::editor {

static constexpr u32 TriangleVertices = 3;

class MikktspaceAdapter {
  struct MeshData {
    const std::vector<glm::vec3> &positions;
    const std::vector<glm::vec3> &normals;
    const std::vector<glm::vec2> &texCoords;
    const std::vector<u32> &indices;
    std::vector<glm::vec4> &tangents;
  };

public:
  MikktspaceAdapter();

  void generate(const std::vector<glm::vec3> &positions,
                const std::vector<glm::vec3> &normals,
                const std::vector<glm::vec2> &texCoords,
                const std::vector<u32> &indices,
                std::vector<glm::vec4> &tangents);

private:
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)

  static int getNumFaces(const SMikkTSpaceContext *pContext);

  static inline int getNumVerticesOfFace(const SMikkTSpaceContext *pContext,
                                         const int faceIndex) {
    return TriangleVertices;
  }

  static void getPosition(const SMikkTSpaceContext *context, f32 outAttribute[],
                          const int faceIndex, const int vertexIndex);

  static void getNormal(const SMikkTSpaceContext *context, f32 outAttribute[],
                        const int faceIndex, const int vertexIndex);

  static void getTexCoord(const SMikkTSpaceContext *context, f32 outAttribute[],
                          const int faceIndex, const int vertexIndex);

  static void setTSpaceBasic(const SMikkTSpaceContext *context,
                             const f32 tangents[], const f32 sign,
                             const int faceIndex, const int vertexIndex);

  // NOLINTEND(cppcoreguidelines-avoid-c-arrays)
private:
  static inline MeshData &getMeshData(const SMikkTSpaceContext *context) {
    return *static_cast<MeshData *>(context->m_pUserData);
  }

  static usize getVertexIndex(const SMikkTSpaceContext *context, int faceIndex,
                              int vertexIndex);

private:
  SMikkTSpaceInterface mInterface{};
};

} // namespace quoll::editor

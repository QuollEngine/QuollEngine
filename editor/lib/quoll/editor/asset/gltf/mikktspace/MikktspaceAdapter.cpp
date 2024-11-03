#include "quoll/core/Base.h"
#include "MikktspaceAdapter.h"

namespace quoll::editor {

MikktspaceAdapter::MikktspaceAdapter() {
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

void MikktspaceAdapter::generate(const std::vector<glm::vec3> &positions,
                                 const std::vector<glm::vec3> &normals,
                                 const std::vector<glm::vec2> &texCoords,
                                 const std::vector<u32> &indices,
                                 std::vector<glm::vec4> &tangents) {
  MeshData data{positions, normals, texCoords, indices, tangents};

  SMikkTSpaceContext context{};
  context.m_pInterface = &mInterface;
  context.m_pUserData = &data;

  genTangSpaceDefault(&context);
}

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
int MikktspaceAdapter::getNumFaces(const SMikkTSpaceContext *pContext) {
  auto &data = getMeshData(pContext);

  return static_cast<int>(data.indices.size() / TriangleVertices);
}

void MikktspaceAdapter::getPosition(const SMikkTSpaceContext *context,
                                    f32 outAttribute[], const int faceIndex,
                                    const int vertexIndex) {
  auto index = getVertexIndex(context, faceIndex, vertexIndex);
  const auto &p = getMeshData(context).positions.at(index);

  outAttribute[0] = p.x;
  outAttribute[1] = p.y;
  outAttribute[2] = p.z;
}

void MikktspaceAdapter::getNormal(const SMikkTSpaceContext *context,
                                  f32 outAttribute[], const int faceIndex,
                                  const int vertexIndex) {
  auto index = getVertexIndex(context, faceIndex, vertexIndex);
  const auto &n = getMeshData(context).normals.at(index);

  outAttribute[0] = n.x;
  outAttribute[1] = n.y;
  outAttribute[2] = n.z;
}

void MikktspaceAdapter::getTexCoord(const SMikkTSpaceContext *context,
                                    f32 outAttribute[], const int faceIndex,
                                    const int vertexIndex) {
  auto index = getVertexIndex(context, faceIndex, vertexIndex);
  const auto &tc = getMeshData(context).texCoords.at(index);

  outAttribute[0] = tc.x;
  outAttribute[1] = tc.y;
}

void MikktspaceAdapter::setTSpaceBasic(const SMikkTSpaceContext *context,
                                       const f32 tangents[], const f32 sign,
                                       const int faceIndex,
                                       const int vertexIndex) {
  auto index = getVertexIndex(context, faceIndex, vertexIndex);
  auto &t = getMeshData(context).tangents.at(index);

  t.x = tangents[0];
  t.y = tangents[1];
  t.z = tangents[2];
  t.w = sign;
}
// NOLINTEND(cppcoreguidelines-avoid-c-arrays)

usize MikktspaceAdapter::getVertexIndex(const SMikkTSpaceContext *context,
                                        int faceIndex, int vertexIndex) {
  auto &meshData = getMeshData(context);
  const usize index = static_cast<usize>(faceIndex * 3 + vertexIndex);

  return meshData.indices.at(index);
}

} // namespace quoll::editor

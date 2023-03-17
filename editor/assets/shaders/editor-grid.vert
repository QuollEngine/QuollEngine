#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 outNearPoint;
layout(location = 1) out vec3 outFarPoint;
layout(location = 2) out mat4 outViewProj;

#include "bindless-editor.glsl"

layout(set = 1, binding = 0) uniform DrawParameters {
  uint gizmoTransforms;
  uint skeletonTransforms;
  uint debugSkeletons;
  uint collidbaleParams;
  uint camera;
  uint gridData;
}
uDrawParams;

const float GRID_SIZE = 1.0;
const vec3 PLANE_VERTICES[4] =
    vec3[](vec3(1.0, 1.0, 0.0), vec3(-1.0, -1.0, 0.0), vec3(-1.0, 1.0, 0.0),
           vec3(1.0, -1.0, 0.0));
const int PLANE_INDICES[6] = int[](0, 1, 2, 1, 0, 3);

/**
 * Unproject grid point with specified
 * Z position.
 *
 * @param point XY position of point
 * @param z Z position of point
 * @return Unprojected position
 */
vec3 unprojectPoint(vec2 point, float z) {
  mat4 invView = inverse(getCamera().view);
  mat4 invProj = inverse(getCamera().proj);
  vec4 unprojectedPoint = invView * invProj * vec4(point, z, 1.0);
  return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
  vec3 vpos = PLANE_VERTICES[PLANE_INDICES[gl_VertexIndex]] * GRID_SIZE;

  outNearPoint = unprojectPoint(vpos.xy, 0.0);
  outFarPoint = unprojectPoint(vpos.xy, 1.0);
  outViewProj = getCamera().viewProj;

  gl_Position = vec4(vpos, 1.0);
}

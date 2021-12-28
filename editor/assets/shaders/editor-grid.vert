#version 450
#extension GL_ARB_separate_shader_objects : enable

float gridSize = 1.0;

const vec3 pos[4] = vec3[](vec3(1.0, 1.0, 0.0), vec3(-1.0, -1.0, 0.0),
                           vec3(-1.0, 1.0, 0.0), vec3(1.0, -1.0, 0.0));

const int indices[6] = int[](0, 1, 2, 1, 0, 3);

layout(location = 0) out vec3 nearPoint;
layout(location = 1) out vec3 farPoint;
layout(location = 2) out mat4 viewProj;

layout(set = 0, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
cameraData;

/**
 * Unproject grid point with specified
 * Z position.
 *
 * @param point XY position of point
 * @param z Z position of point
 */
vec3 unprojectPoint(vec2 point, float z) {
  mat4 invView = inverse(cameraData.view);
  mat4 invProj = inverse(cameraData.proj);
  vec4 unprojectedPoint = invView * invProj * vec4(point, z, 1.0);
  return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
  vec3 vpos = pos[indices[gl_VertexIndex]] * gridSize;

  nearPoint = unprojectPoint(vpos.xy, 0.0);
  farPoint = unprojectPoint(vpos.xy, 1.0);
  viewProj = cameraData.viewProj;

  gl_Position = vec4(vpos, 1.0);
}

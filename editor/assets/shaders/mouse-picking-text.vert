#version 460
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out flat uint64_t outEntity;

#include "mouse-picking-base.glsl"

layout(push_constant) uniform PushConstants { vec4 bounds; }
uTextParams;

void main() {
  mat4 modelMatrix = getTextTransform(gl_BaseInstance).modelMatrix;

  const vec2 positions[4] =
      vec2[](vec2(uTextParams.bounds.x, uTextParams.bounds.y),
             vec2(uTextParams.bounds.z, uTextParams.bounds.y),
             vec2(uTextParams.bounds.x, uTextParams.bounds.w),
             vec2(uTextParams.bounds.z, uTextParams.bounds.w));

  vec2 vertex = positions[gl_VertexIndex];

  gl_Position = getCamera().viewProj * modelMatrix * vec4(vertex, 0.0, 1.0);

  outEntity = uDrawParams.textEntities.entities[gl_BaseInstance];
}

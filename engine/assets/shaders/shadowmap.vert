#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;

#include "bindless-base.glsl"

layout(push_constant) uniform PushConstants { ivec4 index; }
pcShadowRef;

void main() {
  mat4 modelMatrix = getMeshTransform(gl_InstanceIndex).modelMatrix;

  gl_Position = getShadowMap(pcShadowRef.index.x).shadowMatrix * modelMatrix *
                vec4(inPosition, 1.0);
  gl_Layer = pcShadowRef.index.x;
}

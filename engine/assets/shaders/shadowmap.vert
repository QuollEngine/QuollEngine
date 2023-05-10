#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;

#include "bindless/base.glsl"
#include "bindless/mesh.glsl"
#include "bindless/shadows.glsl"

layout(set = 0, binding = 0) uniform DrawParameters {
  TransformsArray meshTransforms;
  TransformsArray skinnedMeshTransforms;
  SkeletonsArray skeletons;
  ShadowMapsArray shadows;
}
uDrawParams;

layout(push_constant) uniform PushConstants { uvec4 shadow; }
pcShadowParams;

void main() {
  mat4 modelMatrix = getMeshTransform(gl_InstanceIndex).modelMatrix;

  gl_Position = getShadowMap(pcShadowParams.shadow.x).shadowMatrix *
                modelMatrix * vec4(inPosition, 1.0);
  gl_Layer = int(pcShadowParams.shadow.x);
}

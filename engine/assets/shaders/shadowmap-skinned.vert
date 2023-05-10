#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

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
  mat4 modelMatrix = getSkinnedMeshTransform(gl_InstanceIndex).modelMatrix;
  SkeletonItem item = getSkeleton(gl_InstanceIndex);

  mat4 skinMatrix = inWeights.x * item.joints[inJoints.x] +
                    inWeights.y * item.joints[inJoints.y] +
                    inWeights.z * item.joints[inJoints.z] +
                    inWeights.w * item.joints[inJoints.w];

  gl_Position = getShadowMap(pcShadowParams.shadow.x).shadowMatrix *
                modelMatrix * skinMatrix * vec4(inPosition, 1.0);
  gl_Layer = int(pcShadowParams.shadow.x);
}

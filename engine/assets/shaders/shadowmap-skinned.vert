#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_viewport_layer_array : enable

layout(location = 0) in vec3 inPosition;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

#include "bindless-base.glsl"

void main() {
  mat4 modelMatrix = getSkinnedMeshTransform(gl_InstanceIndex).modelMatrix;
  SkeletonItem item = getSkeleton(gl_InstanceIndex);

  mat4 skinMatrix = inWeights.x * item.joints[inJoints.x] +
                    inWeights.y * item.joints[inJoints.y] +
                    inWeights.z * item.joints[inJoints.z] +
                    inWeights.w * item.joints[inJoints.w];

  gl_Position = getShadowMap(pcDrawParameters.index9).shadowMatrix *
                modelMatrix * skinMatrix * vec4(inPosition, 1.0);
  gl_Layer = int(pcDrawParameters.index9);
}

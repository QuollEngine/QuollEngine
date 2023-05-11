#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

#include "bindless-editor.glsl"

layout(set = 0, binding = 0) uniform DrawParameters {
  Empty gizmoTransforms;
  Empty skeletonTransforms;
  Empty debugSkeletons;
  Empty collidableParams;
  Camera camera;
  Empty gridData;
  TransformsArray outlineTransforms;
  SkeletonsArray outlineSkeletons;
}
uDrawParams;

#define getOutlineTransform(index) uDrawParams.outlineTransforms.items[index]

#define getOutlineSkeleton(index) uDrawParams.outlineSkeletons.items[index]

/**
 * @brief Push constant for color
 */
layout(push_constant) uniform PushConstants {
  vec4 color;
  vec4 scale;
  uvec4 index;
}
pcOutline;

void main() {
  SkeletonItem item = getOutlineSkeleton(0);

  mat4 skinMatrix = inWeights.x * item.joints[inJoints.x] +
                    inWeights.y * item.joints[inJoints.y] +
                    inWeights.z * item.joints[inJoints.z] +
                    inWeights.w * item.joints[inJoints.w];

  gl_Position = getCamera().viewProj *
                getOutlineTransform(gl_InstanceIndex).modelMatrix * skinMatrix *
                vec4(inPosition * pcOutline.scale.x, 1.0);
}

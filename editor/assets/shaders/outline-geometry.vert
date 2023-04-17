#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 6) in uvec4 inJoints;
layout(location = 7) in vec4 inWeights;

#include "bindless-editor.glsl"

#define getOutlineTransform(index)                                             \
  GetBindlessResource(TransformData, uDrawParams.outlineTransforms).items[index]

layout(set = 1, binding = 0) uniform DrawParameters {
  uint gizmoTransforms;
  uint skeletonTransforms;
  uint debugSkeletons;
  uint collidableParams;
  uint camera;
  uint gridData;
  uint outlineTransforms;
  uint outlineSkeletons;
}
uDrawParams;

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
  gl_Position = getCamera().viewProj *
                getOutlineTransform(gl_InstanceIndex).modelMatrix *
                vec4(inPosition * pcOutline.scale.x, 1.0);
}

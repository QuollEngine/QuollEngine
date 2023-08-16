#version 460

layout(location = 0) in vec3 inPosition;

#include "bindless-editor.glsl"

layout(set = 0, binding = 0) uniform DrawParameters {
  Empty gizmoTransforms;
  Empty skeletonTransforms;
  Empty debugSkeletons;
  Empty collidableParams;
  Camera camera;
  Empty gridData;
  TransformsArray outlineTransforms;
  Empty outlineSkeletons;
}
uDrawParams;

#define getOutlineTransform(index) uDrawParams.outlineTransforms.items[index]

/**
 * @brief Push constant for color
 */
layout(push_constant) uniform PushConstants {
  vec4 color;
  vec4 scale;
  uvec4 index;
}
uOutline;

void main() {
  gl_Position = getCamera().viewProj *
                getOutlineTransform(gl_InstanceIndex).modelMatrix *
                vec4(inPosition * uOutline.scale.x, 1.0);
}

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
pcOutline;

const vec2 positions[4] =
    vec2[](vec2(-1, -1), vec2(+1, -1), vec2(-1, +1), vec2(+1, +1));

void main() {
  gl_Position = getCamera().viewProj *
                getOutlineTransform(gl_InstanceIndex).modelMatrix *
                vec4(positions[gl_VertexIndex] * pcOutline.scale.x, 0.0, 1.0);
}
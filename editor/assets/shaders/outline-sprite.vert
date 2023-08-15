#version 460

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
  Empty glyphs;
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

const vec2 positions[4] = vec2[](vec2(-0.5, -0.5), vec2(+0.5, -0.5),
                                 vec2(-0.5, +0.5), vec2(+0.5, +0.5));

void main() {
  gl_Position = getCamera().viewProj *
                getOutlineTransform(gl_InstanceIndex).modelMatrix *
                vec4(positions[gl_VertexIndex] * uOutline.scale.x, 0.0, 1.0);
}

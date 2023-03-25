#version 450 core

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

#include "bindless-editor.glsl"

layout(set = 1, binding = 0) uniform sampler2D uGlobalTextures[];

layout(set = 1, binding = 0) uniform DrawParameters {
  uint gizmoTransforms;
  uint skeletonTransforms;
  uint debugSkeletons;
  uint collidableParams;
  uint camera;
  uint gridData;
  uint pad0;
  uint pad1;
}
uDrawParams;

/**
 * @brief Push for icons
 */
layout(push_constant) uniform IconParams { uvec4 icon; }
pcIconParams;

void main() {
  outColor = texture(uGlobalTextures[pcIconParams.icon.x], inTexCoord);
  gl_FragDepth = 0.0;
}

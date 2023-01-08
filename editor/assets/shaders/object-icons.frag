#version 450 core

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

#include "bindless-editor.glsl"

layout(set = 1, binding = 0) uniform sampler2D uGlobalTextures[];

void main() {
  outColor = texture(getGizmoIcon(), inTexCoord);
  gl_FragDepth = 0.0;
}

#version 450 core

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D uTexture;

void main() {
  outColor = texture(uTexture, inTexCoord);
  gl_FragDepth = 0.0;
}

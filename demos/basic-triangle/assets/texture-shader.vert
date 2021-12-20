#version 450

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 3) in vec3 vColor;
layout(location = 4) in vec2 vTextureCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTextureCoord;

void main() {
  gl_Position = vec4(vPosition, 1.0f);
  outColor = vColor;
  outTextureCoord = vTextureCoord;
}
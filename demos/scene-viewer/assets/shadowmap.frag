#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(std140, set = 1, binding = 0) uniform MaterialData {
  mat4 lightMatrix;
  uint lightIndex[1];
}
materialData;

void main() { outColor = vec4(1.0, 0.0, 0.0, 1.0); }
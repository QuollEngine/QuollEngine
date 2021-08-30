#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 texCoord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform samplerCube texCube;

void main() { outColor = texture(texCube, texCoord); }

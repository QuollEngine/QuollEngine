#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTextureCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D uTexture;

void main() {
  vec3 texColor = texture(uTexture, inTextureCoord).xyz;
  outColor = vec4(texColor, 1.0);
}

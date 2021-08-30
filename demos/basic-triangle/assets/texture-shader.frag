#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 textureCoord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D tex;

void main() {
  vec3 texColor = texture(tex, textureCoord).xyz;
  outColor = vec4(texColor, 1.0);
}

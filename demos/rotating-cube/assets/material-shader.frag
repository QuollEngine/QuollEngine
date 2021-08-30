#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 2) in float albedo;
layout(location = 3) in float specular;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform MaterialBuffer {
  vec4 diffuse;
  float shininess;
  vec4 specular;
}
materialData;

layout(set = 1, binding = 1) uniform sampler2D tex;

vec4 lightSpecular = vec4(1.0, 1.0, 1.0, 1.0);

void main() {
  vec4 specularp = specular * lightSpecular * materialData.specular;
  vec4 albedop = albedo * materialData.diffuse;

  outColor = albedop + specularp;
}

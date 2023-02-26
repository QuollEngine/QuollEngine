#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 inTexCoord;
layout(location = 1) in flat uint inTextureIndex;

layout(location = 0) out vec4 outColor;

#include "bindless-base.glsl"

layout(set = 1, binding = 0) uniform samplerCube uGlobalTextures[];

void main() {
  vec3 color =
      texture(uGlobalTextures[pcDrawParameters.index9], inTexCoord).xyz;

  outColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}

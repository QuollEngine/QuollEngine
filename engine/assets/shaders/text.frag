#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 outColor;

#include "bindless/base.glsl"

layout(set = 0, binding = 0) uniform sampler2D uGlobalTextures[];

layout(push_constant) uniform PushConstants { uvec4 text; }
pcTextParams;

float median(vec3 msd) {
  return max(min(msd.r, msd.g), min(max(msd.r, msd.g), msd.b));
}

float screenPxRange() {
  vec2 unitRange =
      vec2(2.0) / vec2(textureSize(uGlobalTextures[pcTextParams.text.x], 0));
  vec2 screenTexSize = vec2(1.0) / fwidth(texCoord);
  return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
  vec3 msd = texture(uGlobalTextures[pcTextParams.text.x], texCoord).rgb;
  float sd = median(msd);
  float screenPxDistance = screenPxRange() * (sd - 0.5);
  float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

  outColor = vec4(1.0, 1.0, 1.0, opacity);
}

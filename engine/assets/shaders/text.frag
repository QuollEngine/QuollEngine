#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 outColor;

#include "bindless/base.glsl"

layout(set = 0, binding = 0) uniform texture2D uGlobalTextures[];
layout(set = 0, binding = 1) uniform sampler uGlobalSamplers[];
layout(set = 0, binding = 2) writeonly uniform image2D uGlobalImages[];

layout(push_constant) uniform PushConstants { uvec4 text; }
uTextParams;

float median(vec3 msd) {
  return max(min(msd.r, msd.g), min(max(msd.r, msd.g), msd.b));
}

float screenPxRange() {
  vec2 unitRange =
      vec2(2.0) /
      vec2(textureSize(sampler2D(uGlobalTextures[uTextParams.text.x],
                                 uGlobalSamplers[uTextParams.text.y]),
                       0));
  vec2 screenTexSize = vec2(1.0) / fwidth(texCoord);
  return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
  vec3 msd = texture(sampler2D(uGlobalTextures[uTextParams.text.x],
                               uGlobalSamplers[uTextParams.text.y]),
                     texCoord)
                 .rgb;
  float sd = median(msd);
  float screenPxDistance = screenPxRange() * (sd - 0.5);
  float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

  outColor = vec4(1.0, 1.0, 1.0, opacity);
}

#version 460
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform texture2D uGlobalTextures[];
layout(set = 1, binding = 1) uniform sampler uGlobalSamplers[];
layout(set = 1, binding = 2) writeonly uniform image2D uGlobalImages[];

/**
 * @brief Push constant outlines
 */
layout(push_constant) uniform PushConstants {
  vec4 color;
  vec4 scale;
  uvec4 index;
}
uOutline;

float median(vec3 msd) {
  return max(min(msd.r, msd.g), min(max(msd.r, msd.g), msd.b));
}

float screenPxRange() {
  vec2 unitRange =
      vec2(2.0) / vec2(textureSize(sampler2D(uGlobalTextures[uOutline.index.z],
                                             uGlobalSamplers[uOutline.index.w]),
                                   0));
  vec2 screenTexSize = vec2(1.0) / fwidth(inTexCoord);
  return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
  vec3 msd = texture(sampler2D(uGlobalTextures[uOutline.index.z],
                               uGlobalSamplers[uOutline.index.w]),
                     inTexCoord)
                 .rgb;
  float sd = median(msd);
  float screenPxDistance = screenPxRange() * (sd - 0.5);
  float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

  if (opacity > 0.0) {
    outColor =
        vec4(uOutline.color.x, uOutline.color.y, uOutline.color.z, opacity);
  } else {
    discard;
  }
}

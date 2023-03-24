#version 450 core
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D uGlobalTextures[];

layout(push_constant) uniform DrawParameters { uvec4 index; };

const vec3 Gamma = vec3(1.0 / 2.2);

/**
 * @brief ACES tonemapping
 *
 * @param color RGB color
 * @return Tonemapped color
 */
vec3 aces(vec3 color) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0,
               1.0);
}

void main() {
  vec4 hdrColor = texture(uGlobalTextures[index.x], inTexCoord);

  outColor = vec4(aces(hdrColor.rgb), hdrColor.a);
}

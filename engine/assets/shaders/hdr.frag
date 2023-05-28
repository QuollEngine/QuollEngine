#version 450 core
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D uGlobalTextures[];
layout(set = 0, binding = 1) writeonly uniform image2D uGlobalImages[];

layout(push_constant) uniform DrawParameters { uvec4 index; };

const vec3 Gamma = vec3(1.0 / 2.2);

const mat3 aces_input_matrix =
    mat3(0.59719f, 0.35458f, 0.04823f, 0.07600f, 0.90834f, 0.01566f, 0.02840f,
         0.13383f, 0.83777f);

const mat3 aces_output_matrix =
    mat3(1.60475f, -0.53108f, -0.07367f, -0.10208f, 1.10813f, -0.00605f,
         -0.00327f, -0.07276f, 1.07602f);

vec3 mul(mat3 m, vec3 v) {
  float x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
  float y = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
  float z = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];
  return vec3(x, y, z);
}

vec3 rtt_and_odt_fit(vec3 v) {
  vec3 a = v * (v + 0.0245786) - 0.000090537;
  vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
  return a / b;
}

vec3 aces(vec3 v) {
  v = mul(aces_input_matrix, v);
  v = rtt_and_odt_fit(v);
  return mul(aces_output_matrix, v);
}

const float BloomContribution = 0.2;

void main() {
  vec4 hdrColor = texture(uGlobalTextures[index.x], inTexCoord);
  vec3 bloomColor = texture(uGlobalTextures[index.y], inTexCoord).rgb;

  outColor =
      vec4(aces(mix(hdrColor.rgb, bloomColor, BloomContribution)), hdrColor.a);
}

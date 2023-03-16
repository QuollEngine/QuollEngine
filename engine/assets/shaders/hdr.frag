#version 450 core
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D uGlobalTextures[];

layout(push_constant) uniform DrawParameters { uvec4 index; };

const vec3 Gamma = vec3(1.0 / 2.2);

void main() {
  vec4 hdrColor = texture(uGlobalTextures[index.x], inTexCoord);

  vec3 color = hdrColor.rgb / (hdrColor.rgb + vec3(1.0));
  color = pow(color, Gamma);

  outColor = vec4(color, hdrColor.a);
}

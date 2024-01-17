#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "bindless/base.glsl"
#include "bindless/camera.glsl"
#include "bindless/transform.glsl"

Buffer(16) SpritesArray { uvec4 texture; };

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in flat uint inTextureIndex;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform texture2D uGlobalTextures[];
layout(set = 0, binding = 1) uniform sampler uGlobalSamplers[];
layout(set = 0, binding = 2) writeonly uniform image2D uGlobalImages[];

layout(set = 1, binding = 0) uniform DrawParameters {
  Empty camera;
  Empty transforms;
  SpritesArray sprites;
  uint defaultSampler;
}
uDrawParams;

void main() {
  outColor = texture(sampler2D(uGlobalTextures[inTextureIndex],
                               uGlobalSamplers[uDrawParams.defaultSampler]),
                     inTexCoord);
}

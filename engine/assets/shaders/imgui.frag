#version 450 core
#extension GL_EXT_nonuniform_qualifier : enable

#include "bindless/base.glsl"

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D uGlobalTextures[];
layout(set = 0, binding = 1) writeonly uniform image2D uGlobalImages[];

layout(push_constant) uniform TextureData { layout(offset = 64) uint index; }
pcTextureData;

void main() {
  outColor =
      inColor * texture(uGlobalTextures[pcTextureData.index], inTexCoord);
}

#version 450 core
#extension GL_EXT_nonuniform_qualifier : enable

#include "bindless/base.glsl"

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform texture2D uGlobalTextures[];
layout(set = 0, binding = 1) uniform sampler uGlobalSamplers[];
layout(set = 0, binding = 2) writeonly uniform image2D uGlobalImages[];

layout(push_constant) uniform TextureData { layout(offset = 64) uvec4 index; }
uTextureData;

void main() {
  outColor = inColor * texture(sampler2D(uGlobalTextures[uTextureData.index.x],
                                         uGlobalSamplers[uTextureData.index.y]),
                               inTexCoord);
}

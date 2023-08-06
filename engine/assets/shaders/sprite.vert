#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "bindless/base.glsl"
#include "bindless/transform.glsl"
#include "bindless/camera.glsl"

Buffer(16) SpritesArray { uint items[]; };

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out flat uint outTextureIndex;

layout(set = 0, binding = 0) uniform sampler2D uGlobalTextures[];
layout(set = 0, binding = 1) writeonly uniform image2D uGlobalImages[];

layout(set = 1, binding = 0) uniform DrawParameters {
  Camera camera;
  TransformsArray transforms;
  SpritesArray sprites;
  Empty pad0;
}
uDrawParams;

const vec2 positions[4] = vec2[](vec2(-0.5, -0.5), vec2(+0.5, -0.5),
                                 vec2(-0.5, +0.5), vec2(+0.5, +0.5));
const vec2 texCoords[4] =
    vec2[](vec2(1, 1), vec2(0, 1), vec2(1, 0), vec2(0, 0));

void main() {
  outTexCoord = texCoords[gl_VertexIndex];
  outTextureIndex = uDrawParams.sprites.items[gl_InstanceIndex].x;

  gl_Position = getCamera().viewProj *
                uDrawParams.transforms.items[gl_InstanceIndex].modelMatrix *
                vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 outTexCoord;

#include "bindless/base.glsl"
#include "bindless/camera.glsl"
#include "bindless/text.glsl"

layout(set = 1, binding = 0) uniform DrawParameters {
  TransformsArray textTransforms;
  Camera camera;
  GlyphsArray glyphs;
  Empty pad0;
}
uDrawParams;

layout(push_constant) uniform PushConstants { uvec4 text; }
uTextParams;

const uint QUAD_VERTICES = 6;

void main() {
  mat4 modelMatrix = getTextTransform(gl_BaseInstance).modelMatrix;

  uint boundIndex = gl_VertexIndex % QUAD_VERTICES;
  uint glyphIndex =
      uTextParams.text.y + uint(floor(gl_VertexIndex / QUAD_VERTICES));

  GlyphItem glyph = getGlyph(glyphIndex);

  vec2 texCoords[QUAD_VERTICES] =
      vec2[](glyph.bounds.xy, glyph.bounds.xw, glyph.bounds.zy, glyph.bounds.zy,
             glyph.bounds.xw, glyph.bounds.zw);

  vec2 positions[QUAD_VERTICES] =
      vec2[](glyph.planeBounds.xy, glyph.planeBounds.xw, glyph.planeBounds.zy,
             glyph.planeBounds.zy, glyph.planeBounds.xw, glyph.planeBounds.zw);

  vec2 vertex = positions[boundIndex];
  outTexCoord = texCoords[boundIndex];

  gl_Position = getCamera().viewProj * modelMatrix * vec4(vertex, 0.0, 1.0);
}

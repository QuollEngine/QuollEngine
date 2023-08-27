#version 460

#include "bindless-editor.glsl"

layout(location = 0) out vec2 outTexCoord;

layout(set = 0, binding = 0) uniform DrawParameters {
  Empty gizmoTransforms;
  Empty skeletonTransforms;
  Empty debugSkeletons;
  Empty collidableParams;
  Camera camera;
  Empty gridData;
  TransformsArray outlineTransforms;
  Empty outlineSkeletons;
  GlyphsArray glyphs;
}
uDrawParams;

layout(set = 1, binding = 0) uniform texture2D uGlobalTextures[];
layout(set = 1, binding = 1) uniform sampler uGlobalSamplers[];
layout(set = 1, binding = 2) writeonly uniform image2D uGlobalImages[];

#define getOutlineTransform(index) uDrawParams.outlineTransforms.items[index]

/**
 * @brief Push constant for color
 */
layout(push_constant) uniform PushConstants {
  vec4 color;
  vec4 scale;
  uvec4 index;
}
uOutline;

const uint QUAD_VERTICES = 6;
float OutlineWidth = uOutline.scale.x - 1.0;

void main() {
  mat4 modelMatrix = getOutlineTransform(gl_BaseInstance).modelMatrix;

  uint boundIndex = gl_VertexIndex % QUAD_VERTICES;
  uint glyphIndex =
      uOutline.index.y + uint(floor(gl_VertexIndex / QUAD_VERTICES));

  GlyphItem glyph = getGlyph(glyphIndex);

  vec2 texCoords[QUAD_VERTICES] =
      vec2[](glyph.bounds.xy, glyph.bounds.xw, glyph.bounds.zy, glyph.bounds.zy,
             glyph.bounds.xw, glyph.bounds.zw);

  vec2 positions[QUAD_VERTICES] =
      vec2[](glyph.planeBounds.xy + vec2(-OutlineWidth, -OutlineWidth),
             glyph.planeBounds.xw + vec2(-OutlineWidth, OutlineWidth),
             glyph.planeBounds.zy + vec2(OutlineWidth, -OutlineWidth),
             glyph.planeBounds.zy + vec2(OutlineWidth, -OutlineWidth),
             glyph.planeBounds.xw + vec2(-OutlineWidth, OutlineWidth),
             glyph.planeBounds.zw + vec2(OutlineWidth, OutlineWidth));

  vec2 vertex = positions[boundIndex];
  outTexCoord = texCoords[boundIndex];

  gl_Position = getCamera().viewProj * modelMatrix * vec4(vertex, 0.0, 1.0);
}

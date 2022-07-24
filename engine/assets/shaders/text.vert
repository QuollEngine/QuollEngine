#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 outTexCoord;

layout(set = 0, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
uCameraData;

struct ObjectItem {
  mat4 modelMatrix;
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectData {
  ObjectItem items[];
}
uObjectData;

struct GlyphItem {
  vec4 bounds;
  vec4 planeBounds;
};

layout(set = 2, binding = 0) readonly buffer GlyphData { GlyphItem items[]; }
uGlyphData;

layout(push_constant) uniform TextData { uint glyphStart; }
pcTextData;

const uint QUAD_VERTICES = 6;

void main() {
  mat4 modelMatrix = uObjectData.items[gl_BaseInstance].modelMatrix;

  uint boundIndex = gl_VertexIndex % QUAD_VERTICES;
  uint glyphIndex =
      pcTextData.glyphStart + uint(floor(gl_VertexIndex / QUAD_VERTICES));

  GlyphItem glyph = uGlyphData.items[glyphIndex];

  vec2 texCoords[QUAD_VERTICES] =
      vec2[](glyph.bounds.xy, glyph.bounds.xw, glyph.bounds.zy, glyph.bounds.zy,
             glyph.bounds.xw, glyph.bounds.zw);

  vec2 positions[QUAD_VERTICES] =
      vec2[](glyph.planeBounds.xy, glyph.planeBounds.xw, glyph.planeBounds.zy,
             glyph.planeBounds.zy, glyph.planeBounds.xw, glyph.planeBounds.zw);

  vec2 vertex = positions[boundIndex];

  outTexCoord = texCoords[boundIndex];

  gl_Position = uCameraData.viewProj * modelMatrix * vec4(vertex, 0.0, 1.0);
}

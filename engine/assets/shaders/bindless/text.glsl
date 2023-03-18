#include "transform.glsl"

/**
 * @brief Single glyph data
 */
struct GlyphItem {
  /**
   * Glyph atlas bounds
   */
  vec4 bounds;

  /**
   * Glyph quad bounds
   */
  vec4 planeBounds;
};

RegisterBuffer(std430, readonly, GlyphData, { GlyphItem items[]; });

#define getTextTransform(index)                                                \
  GetBindlessResource(TransformData, uDrawParams.textTransforms).items[index]

#define getGlyph(index)                                                        \
  GetBindlessResource(GlyphData, uDrawParams.glyphs).items[index]

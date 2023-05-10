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

Buffer(16) GlyphsArray { GlyphItem items[]; };

#define getTextTransform(index) uDrawParams.textTransforms.items[index]

#define getGlyph(index) uDrawParams.glyphs.items[index]

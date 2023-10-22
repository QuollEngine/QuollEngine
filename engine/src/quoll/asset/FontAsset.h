#pragma once

namespace quoll {

/**
 * @brief Font glyph data
 */
struct FontGlyph {

  /**
   * Atlas bounds
   */
  glm::vec4 bounds;

  /**
   * Quad bounds
   */
  glm::vec4 planeBounds;

  /**
   * Glyph advance
   */
  f32 advanceX = 0.0;
};

/**
 * @brief Font asset data
 */
struct FontAsset {
  /**
   * Font atlas raw data
   */
  std::vector<std::byte> atlas;

  /**
   * Font atlas dimensions
   */
  glm::uvec2 atlasDimensions;

  /**
   * Glyph data
   */
  std::unordered_map<u32, FontGlyph> glyphs;

  /**
   * Font scale
   */
  f32 fontScale = 1.0f;

  /**
   * Device handle
   */
  rhi::TextureHandle deviceHandle = rhi::TextureHandle::Null;
};

} // namespace quoll

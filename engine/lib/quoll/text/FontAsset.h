#pragma once

#include "quoll/asset/AssetType.h"
#include "quoll/rhi/RenderHandle.h"

namespace quoll {

struct FontGlyph {
  glm::vec4 atlasBounds;

  glm::vec4 planeBounds;

  f32 advanceX = 0.0;
};

struct FontAsset {
  std::vector<std::byte> atlasBytes;

  glm::uvec2 atlasDimensions;

  std::unordered_map<u32, FontGlyph> glyphs;

  f32 fontScale = 1.0f;

  usize size = 0;
};

} // namespace quoll

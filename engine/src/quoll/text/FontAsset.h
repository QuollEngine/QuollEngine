#pragma once

#include "quoll/asset/Asset.h"
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

  rhi::TextureHandle deviceHandle = rhi::TextureHandle::Null;
};

} // namespace quoll

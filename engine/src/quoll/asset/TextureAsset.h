#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/Format.h"

namespace quoll {

enum class TextureAssetType { Standard, Cubemap };

struct TextureAssetMipLevel {
  usize offset = 0;

  usize size = 0;

  u32 width = 0;

  u32 height = 0;
};

struct TextureAsset {
  u32 width = 0;

  u32 height = 0;

  u32 layers = 0;

  TextureAssetType type = TextureAssetType::Standard;

  rhi::Format format = rhi::Format::Rgba8Srgb;

  std::vector<u8> data;

  std::vector<TextureAssetMipLevel> levels;

  rhi::TextureHandle deviceHandle = rhi::TextureHandle::Null;
};

} // namespace quoll

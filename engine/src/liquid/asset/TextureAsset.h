#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid {

enum class TextureAssetType { Standard, Cubemap };

static constexpr uint32_t DEFAULT_TEXTURE_FORMAT = 43;

struct TextureAsset {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;
  TextureAssetType type = TextureAssetType::Standard;
  uint32_t format = DEFAULT_TEXTURE_FORMAT;

  void *data = nullptr;
  rhi::TextureHandle deviceHandle = rhi::TextureHandle::Invalid;
};

} // namespace liquid

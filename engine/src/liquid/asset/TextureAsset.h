#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid {

struct TextureAsset {
  uint32_t width = 0;
  uint32_t height = 0;
  void *data = nullptr;
  rhi::TextureHandle deviceHandle = rhi::TextureHandle::Invalid;
};

} // namespace liquid
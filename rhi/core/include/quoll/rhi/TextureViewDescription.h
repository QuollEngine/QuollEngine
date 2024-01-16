#pragma once

#include "RenderHandle.h"

namespace quoll::rhi {

struct TextureViewDescription {
  TextureHandle texture = TextureHandle::Null;

  u32 baseMipLevel = 0;

  u32 mipLevelCount = 1;

  u32 baseLayer = 0;

  u32 layerCount = 1;

  String debugName;
};

} // namespace quoll::rhi

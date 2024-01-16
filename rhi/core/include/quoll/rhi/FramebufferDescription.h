#pragma once

#include "RenderHandle.h"

namespace quoll::rhi {

struct FramebufferDescription {
  RenderPassHandle renderPass = RenderPassHandle::Null;

  u32 width = 0;

  u32 height = 0;

  u32 layers = 0;

  std::vector<TextureHandle> attachments;

  String debugName;
};

} // namespace quoll::rhi

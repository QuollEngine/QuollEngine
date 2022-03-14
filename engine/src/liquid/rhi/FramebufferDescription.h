#pragma once

#include "RenderHandle.h"

namespace liquid::rhi {

struct FramebufferDescription {
  rhi::RenderPassHandle renderPass = 0;
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;
  std::vector<TextureHandle> attachments;
};

} // namespace liquid::rhi

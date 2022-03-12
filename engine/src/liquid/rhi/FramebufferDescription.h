#pragma once

#include "RenderHandle.h"
#include <vulkan/vulkan.hpp>

namespace liquid {

struct FramebufferDescription {
  RenderPassHandle renderPass = 0;
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;
  std::vector<TextureHandle> attachments;
};

} // namespace liquid

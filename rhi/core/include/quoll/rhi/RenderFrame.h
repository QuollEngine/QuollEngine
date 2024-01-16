#pragma once

#include "RenderCommandList.h"

namespace quoll::rhi {

struct RenderFrame {
  u32 frameIndex = std::numeric_limits<u32>::max();

  u32 swapchainImageIndex = std::numeric_limits<u32>::max();

  RenderCommandList &commandList;
};

} // namespace quoll::rhi

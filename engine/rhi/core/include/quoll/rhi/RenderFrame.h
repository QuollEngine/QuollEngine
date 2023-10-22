#pragma once

#include "RenderCommandList.h"

namespace quoll::rhi {

/**
 * @brief Rendering frame details
 */
struct RenderFrame {
  /**
   * Frame index
   */
  u32 frameIndex = std::numeric_limits<u32>::max();

  /**
   * Swapchain image index
   */
  u32 swapchainImageIndex = std::numeric_limits<u32>::max();

  /**
   * Command list
   */
  RenderCommandList &commandList;
};

} // namespace quoll::rhi

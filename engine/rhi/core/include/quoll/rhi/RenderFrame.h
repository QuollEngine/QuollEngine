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
  uint32_t frameIndex = std::numeric_limits<uint32_t>::max();

  /**
   * Swapchain image index
   */
  uint32_t swapchainImageIndex = std::numeric_limits<uint32_t>::max();

  /**
   * Command list
   */
  RenderCommandList &commandList;
};

} // namespace quoll::rhi

#pragma once

namespace liquid::rhi {

/**
 * @brief Rendering frame details
 */
struct RenderFrame {
  /**
   * @brief Frame index
   */
  uint32_t frameIndex = std::numeric_limits<uint32_t>::max();

  /**
   * @brief Swapchain image index
   */
  uint32_t swapchainImageIndex = std::numeric_limits<uint32_t>::max();

  /**
   * @brief Command list
   */
  RenderCommandList &commandList;
};

} // namespace liquid::rhi

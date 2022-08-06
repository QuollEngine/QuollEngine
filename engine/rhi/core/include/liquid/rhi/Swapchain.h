#pragma once

namespace liquid::rhi {

/**
 * @brief Platform agnostic swapchain details
 */
struct Swapchain {
  /**
   * Swapchain textures
   */
  std::vector<TextureHandle> textures{};

  /**
   * Swapchain extent
   */
  glm::uvec2 extent{};
};

} // namespace liquid::rhi

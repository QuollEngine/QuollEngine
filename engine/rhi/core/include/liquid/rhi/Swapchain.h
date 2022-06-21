#pragma once

namespace liquid::rhi {

/**
 * @brief Platform agnostic swapchain details
 */
struct Swapchain {
  /**
   * @brief Swapchain textures
   */
  std::vector<TextureHandle> textures{};

  /**
   * @brief Swapchain extent
   */
  glm::uvec2 extent{};
};

} // namespace liquid::rhi

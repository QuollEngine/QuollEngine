#pragma once

namespace quoll::rhi {

struct Swapchain {
  std::vector<TextureHandle> textures{};

  glm::uvec2 extent{};
};

} // namespace quoll::rhi

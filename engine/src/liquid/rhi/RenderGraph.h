#pragma once

#include "RenderGraphPass.h"

namespace liquid::rhi {

/**
 * @brief Render graph
 */
class RenderGraph {
public:
  /**
   * @brief Add pass
   *
   * @param name Pass name
   * @return Render graph pass
   */
  RenderGraphPass &addPass(const String &name);

  /**
   * @brief Get swapchain handle
   *
   * @return Swapchain handle
   */
  inline TextureHandle getSwapchain() const { return mSwapchain; }

  /**
   * @brief Compile pass
   *
   * @return List of indexes that point to passes
   */
  std::vector<size_t> compile();

  /**
   * @brief Get passes
   *
   * @return Render graph passes
   */
  inline std::vector<RenderGraphPass> &getPasses() { return mPasses; }

  /**
   * @brief Check if texture is swapchain
   *
   * @param handle Texture handle
   * @retval true Texture is swapchain
   * @retval false Texture is not swapchain
   */
  inline bool isSwapchain(TextureHandle handle) const {
    return handle == mSwapchain;
  }

private:
  std::vector<RenderGraphPass> mPasses;
  TextureHandle mSwapchain{1};
};

} // namespace liquid::rhi

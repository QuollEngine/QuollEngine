#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/RenderCommandList.h"
#include "liquid/rhi/Swapchain.h"
#include "ShaderLibrary.h"

namespace liquid {

/**
 * @brief Presents texture to swapchain
 *        using full screen quad
 */
class Presenter {
public:
  /**
   * @brief Create presenter
   *
   * @param shaderLibrary Shader library
   * @param device Render device
   */
  Presenter(ShaderLibrary &shaderLibrary, rhi::RenderDevice *device);

  /**
   * @brief Update framebuffers
   *
   * @param swapchain Swapchain
   */
  void updateFramebuffers(const rhi::Swapchain &swapchain);

  /**
   * @brief Present texture to swapchain
   *
   * @param commandList Command list
   * @param handle Texture handle
   * @param imageIndex Swapchain image index
   */
  void present(rhi::RenderCommandList &commandList, rhi::TextureHandle handle,
               uint32_t imageIndex);

private:
  rhi::RenderDevice *mDevice;
  ShaderLibrary &mShaderLibrary;
  rhi::RenderPassHandle mPresentPass = rhi::RenderPassHandle::Invalid;
  rhi::PipelineHandle mPresentPipeline = rhi::PipelineHandle::Invalid;
  std::vector<rhi::FramebufferHandle> mFramebuffers{};
  glm::uvec2 mExtent{0, 0};
};

} // namespace liquid

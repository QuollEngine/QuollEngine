#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/RenderCommandList.h"
#include "quoll/rhi/Swapchain.h"
#include "RenderStorage.h"

namespace quoll {

/**
 * @brief Presents texture to swapchain
 *        using full screen quad
 */
class Presenter {
public:
  /**
   * @brief Create presenter
   *
   * @param renderStorage Render storage
   */
  Presenter(RenderStorage &renderStorage);

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
               u32 imageIndex);

  /**
   * @brief Set flag to update framebuffers
   */
  void enqueueFramebufferUpdate();

  /**
   * @brief Check if framebuffer update is required
   *
   * @retval true Framebuffer update is required
   * @retval false Framebuffer update is not required
   */
  inline bool requiresFramebufferUpdate() const { return mUpdateRequired; }

private:
  RenderStorage &mRenderStorage;
  rhi::RenderPassHandle mPresentPass = rhi::RenderPassHandle::Null;
  rhi::PipelineHandle mPresentPipeline = rhi::PipelineHandle::Null;
  std::vector<rhi::FramebufferHandle> mFramebuffers{};
  glm::uvec2 mExtent{0, 0};
  rhi::TextureHandle mPresentTexture{0};
  rhi::Descriptor mPresentDescriptor;

  bool mUpdateRequired = false;
};

} // namespace quoll

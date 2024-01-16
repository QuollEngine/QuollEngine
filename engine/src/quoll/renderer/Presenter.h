#pragma once

#include "quoll/rhi/RenderCommandList.h"
#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/Swapchain.h"
#include "RenderStorage.h"

namespace quoll {

class Presenter {
public:
  Presenter(RenderStorage &renderStorage);

  void updateFramebuffers(const rhi::Swapchain &swapchain);

  void present(rhi::RenderCommandList &commandList, rhi::TextureHandle handle,
               u32 imageIndex);

  void enqueueFramebufferUpdate();

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

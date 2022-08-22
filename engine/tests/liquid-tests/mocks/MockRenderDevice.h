#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "MockBuffer.h"

class MockRenderDevice : public liquid::rhi::RenderDevice {
public:
  liquid::rhi::RenderFrame beginFrame();

  void endFrame(const liquid::rhi::RenderFrame &renderFrame);

  void waitForIdle();

  void synchronize(liquid::rhi::ResourceRegistry &registry);

  const liquid::rhi::PhysicalDeviceInformation getDeviceInformation();

  const liquid::rhi::DeviceStats &getDeviceStats() const;

  void destroyResources();

  liquid::rhi::Swapchain getSwapchain();

  liquid::rhi::ShaderHandle
  createShader(const liquid::rhi::ShaderDescription &description);

  liquid::rhi::Buffer
  createBuffer(const liquid::rhi::BufferDescription &description);

  liquid::rhi::TextureHandle
  createTexture(const liquid::rhi::TextureDescription &description);

  const liquid::rhi::TextureDescription
  getTextureDescription(liquid::rhi::TextureHandle handle) const;

  liquid::rhi::RenderPassHandle
  createRenderPass(const liquid::rhi::RenderPassDescription &description);

  void destroyRenderPass(liquid::rhi::RenderPassHandle handle);

  liquid::rhi::FramebufferHandle
  createFramebuffer(const liquid::rhi::FramebufferDescription &description);

  void destroyFramebuffer(liquid::rhi::FramebufferHandle handle);

  inline const MockBuffer &getBuffer(liquid::rhi::BufferHandle handle) {
    return mBuffers.at(handle);
  }

private:
  template <class THandle> inline THandle getNewHandle() {
    auto handle = THandle{mLastHandle};
    mLastHandle++;
    return handle;
  }

private:
  std::unordered_map<liquid::rhi::ShaderHandle, liquid::rhi::ShaderDescription>
      mShaders;
  std::unordered_map<liquid::rhi::BufferHandle, MockBuffer> mBuffers;
  std::unordered_map<liquid::rhi::TextureHandle,
                     liquid::rhi::TextureDescription>
      mTextures;
  std::unordered_map<liquid::rhi::RenderPassHandle,
                     liquid::rhi::RenderPassDescription>
      mRenderPasses;
  std::unordered_map<liquid::rhi::FramebufferHandle,
                     liquid::rhi::FramebufferDescription>
      mFramebuffers;

  uint32_t mLastHandle = 1;

  liquid::rhi::RenderCommandList mNoopCommandList{nullptr};
  liquid::rhi::DeviceStats mNoopDeviceStats{nullptr};
};

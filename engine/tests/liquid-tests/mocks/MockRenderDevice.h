#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "MockBuffer.h"
#include "MockDescriptor.h"

class MockRenderDevice : public liquid::rhi::RenderDevice {
public:
  liquid::rhi::RenderCommandList requestImmediateCommandList();

  void submitImmediate(liquid::rhi::RenderCommandList &commandList);

  liquid::rhi::RenderFrame beginFrame();

  void endFrame(const liquid::rhi::RenderFrame &renderFrame);

  void waitForIdle();

  const liquid::rhi::PhysicalDeviceInformation getDeviceInformation();

  const liquid::rhi::DeviceStats &getDeviceStats() const;

  void destroyResources();

  liquid::rhi::Swapchain getSwapchain();

  liquid::rhi::DescriptorLayoutHandle createDescriptorLayout(
      const liquid::rhi::DescriptorLayoutDescription &description);

  liquid::rhi::Descriptor
  createDescriptor(liquid::rhi::DescriptorLayoutHandle handle);

  liquid::rhi::ShaderHandle
  createShader(const liquid::rhi::ShaderDescription &description);

  liquid::rhi::Buffer
  createBuffer(const liquid::rhi::BufferDescription &description);

  void destroyBuffer(liquid::rhi::BufferHandle handle);

  liquid::rhi::TextureHandle
  createTexture(const liquid::rhi::TextureDescription &description);

  void destroyTexture(liquid::rhi::TextureHandle handle);

  const liquid::rhi::TextureDescription
  getTextureDescription(liquid::rhi::TextureHandle handle) const;

  liquid::rhi::TextureViewHandle
  createTextureView(const liquid::rhi::TextureViewDescription &description);

  void destroyTextureView(liquid::rhi::TextureViewHandle handle);

  liquid::rhi::RenderPassHandle
  createRenderPass(const liquid::rhi::RenderPassDescription &description);

  void destroyRenderPass(liquid::rhi::RenderPassHandle handle);

  liquid::rhi::FramebufferHandle
  createFramebuffer(const liquid::rhi::FramebufferDescription &description);

  void destroyFramebuffer(liquid::rhi::FramebufferHandle handle);

  liquid::rhi::PipelineHandle
  createPipeline(const liquid::rhi::GraphicsPipelineDescription &description);

  liquid::rhi::PipelineHandle
  createPipeline(const liquid::rhi::ComputePipelineDescription &description);

  void destroyPipeline(liquid::rhi::PipelineHandle handle);

  inline const MockBuffer &getBuffer(liquid::rhi::BufferHandle handle) {
    return mBuffers.at(handle);
  }

  size_t addTextureUpdateListener(
      const std::function<void(const std::set<liquid::rhi::TextureHandle> &)>
          &listener);

  void removeTextureUpdateListener(size_t handle);

private:
  template <class THandle> inline THandle getNewHandle() {
    auto handle = THandle{mLastHandle};
    mLastHandle++;
    return handle;
  }

private:
  std::unordered_map<liquid::rhi::DescriptorLayoutHandle,
                     liquid::rhi::DescriptorLayoutDescription>
      mDescriptorLayouts;
  std::unordered_map<liquid::rhi::DescriptorHandle, MockDescriptor>
      mDescriptors;
  std::unordered_map<liquid::rhi::ShaderHandle, liquid::rhi::ShaderDescription>
      mShaders;
  std::unordered_map<liquid::rhi::BufferHandle, MockBuffer> mBuffers;
  std::unordered_map<liquid::rhi::TextureHandle,
                     liquid::rhi::TextureDescription>
      mTextures;

  std::unordered_map<liquid::rhi::TextureViewHandle,
                     liquid::rhi::TextureViewDescription>
      mTextureViews;

  std::unordered_map<liquid::rhi::RenderPassHandle,
                     liquid::rhi::RenderPassDescription>
      mRenderPasses;
  std::unordered_map<liquid::rhi::FramebufferHandle,
                     liquid::rhi::FramebufferDescription>
      mFramebuffers;

  std::unordered_map<liquid::rhi::PipelineHandle,
                     liquid::rhi::GraphicsPipelineDescription>
      mPipelines;

  std::unordered_map<liquid::rhi::PipelineHandle,
                     liquid::rhi::ComputePipelineDescription>
      mComputePipelines;

  uint32_t mLastHandle = 1;

  liquid::rhi::RenderCommandList mNoopCommandList{nullptr};
  liquid::rhi::DeviceStats mNoopDeviceStats{nullptr};
};

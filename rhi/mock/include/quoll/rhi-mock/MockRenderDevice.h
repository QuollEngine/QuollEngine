#pragma once

#include "quoll/rhi/RenderDevice.h"
#include "MockBuffer.h"
#include "MockCommandList.h"
#include "MockDescriptor.h"
#include "MockPipeline.h"
#include "MockResourceMap.h"
#include "MockTexture.h"

namespace quoll::rhi {

class MockRenderDevice : public RenderDevice {
public:
  MockRenderDevice();

  inline MockBuffer *getBuffer(BufferHandle handle) {
    return mBuffers.at(handle).get();
  }

  inline void
  setTimestampCollectorFn(std::function<void(std::vector<u64> &)> &&fn) {
    mTimestampCollectorFn = fn;
  }

public:
  RenderCommandList requestImmediateCommandList() override;

  void submitImmediate(RenderCommandList &commandList) override;

  RenderFrame beginFrame() override;

  void endFrame(const RenderFrame &renderFrame) override;

  void collectTimestamps(std::vector<u64> &timestamps) override;

  void waitForIdle() override;

  const PhysicalDeviceInformation getDeviceInformation() override;

  const DeviceStats &getDeviceStats() const override;

  void destroyResources() override;

  Swapchain getSwapchain() override;

  void recreateSwapchain() override;

  void createShader(const ShaderDescription &description,
                    ShaderHandle handle) override;

  DescriptorLayoutHandle createDescriptorLayout(
      const DescriptorLayoutDescription &description) override;

  Descriptor createDescriptor(DescriptorLayoutHandle layout) override;

  Buffer createBuffer(const BufferDescription &description) override;

  void destroyBuffer(BufferHandle handle) override;

  void createTexture(const TextureDescription &description,
                     rhi::TextureHandle handle) override;

  const TextureDescription
  getTextureDescription(TextureHandle handle) const override;

  const TextureViewDescription
  getTextureViewDescription(TextureHandle handle) const;

  inline bool hasTexture(TextureHandle handle) const {
    return mTextures.exists(handle);
  }

  void destroyTexture(TextureHandle handle) override;

  u32 getTextureUpdates(TextureHandle handle);

  void createTextureView(const TextureViewDescription &description,
                         TextureHandle handle) override;

  void createSampler(const SamplerDescription &description,
                     SamplerHandle handle) override;

  void destroySampler(SamplerHandle handle) override;

  void createRenderPass(const RenderPassDescription &description,
                        RenderPassHandle handle) override;

  void destroyRenderPass(RenderPassHandle handle) override;

  const RenderPassDescription
  getRenderPassDescription(RenderPassHandle handle) const;

  inline bool hasRenderPass(RenderPassHandle handle) const {
    return mRenderPasses.exists(handle);
  }

  void createFramebuffer(const FramebufferDescription &description,
                         FramebufferHandle handle) override;

  void destroyFramebuffer(FramebufferHandle handle) override;

  const FramebufferDescription
  getFramebufferDescription(FramebufferHandle handle) const;

  inline bool hasFramebuffer(FramebufferHandle handle) const {
    return mFramebuffers.exists(handle);
  }

  void createPipeline(const GraphicsPipelineDescription &description,
                      PipelineHandle handle) override;

  void createPipeline(const ComputePipelineDescription &description,
                      PipelineHandle handle) override;

  void destroyPipeline(PipelineHandle handle) override;

  const MockPipeline &getPipeline(PipelineHandle handle) const;

  inline bool hasPipeline(PipelineHandle handle) override {
    return mPipelines.exists(handle);
  }

private:
  MockResourceMap<BufferHandle, std::unique_ptr<MockBuffer>> mBuffers;
  MockResourceMap<TextureHandle, MockTexture> mTextures;
  MockResourceMap<SamplerHandle, SamplerDescription> mSamplers;
  MockResourceMap<FramebufferHandle, FramebufferDescription> mFramebuffers;
  MockResourceMap<RenderPassHandle, RenderPassDescription> mRenderPasses;

  MockResourceMap<ShaderHandle, ShaderDescription> mShaders;
  MockResourceMap<DescriptorLayoutHandle, DescriptorLayoutDescription>
      mDescriptorLayouts;
  MockResourceMap<DescriptorHandle, std::unique_ptr<MockDescriptor>>
      mDescriptors;
  MockResourceMap<PipelineHandle, MockPipeline> mPipelines;

  std::array<RenderCommandList, NumFrames> mCommandLists;
  std::vector<MockCommandList> mSubmittedCommandLists;
  u32 mFrameIndex = 0;

  DeviceStats mDeviceStats;

  std::function<void(std::vector<u64> &)> mTimestampCollectorFn = [](auto &) {};
};

} // namespace quoll::rhi

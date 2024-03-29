#pragma once

#include "quoll/rhi/DeviceStats.h"
#include "quoll/rhi/RenderDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDeviceObject.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanPipelineLayoutCache.h"
#include "VulkanQueue.h"
#include "VulkanRenderBackend.h"
#include "VulkanRenderContext.h"
#include "VulkanResourceAllocator.h"
#include "VulkanResourceRegistry.h"
#include "VulkanSwapchain.h"
#include "VulkanTimestampManager.h"
#include "VulkanUploadContext.h"

namespace quoll::rhi {

class VulkanRenderDevice : public RenderDevice {
public:
  VulkanRenderDevice(VulkanRenderBackend &backend,
                     const VulkanPhysicalDevice &physicalDevice);

  RenderCommandList requestImmediateCommandList() override;

  void submitImmediate(RenderCommandList &commandList) override;

  RenderFrame beginFrame() override;

  void endFrame(const RenderFrame &renderFrame) override;

  void collectTimestamps(std::vector<u64> &timestamps) override;

  void waitForIdle() override;

  const PhysicalDeviceInformation getDeviceInformation() override {
    return mPhysicalDevice.getDeviceInfo();
  }

  void destroyResources() override;

  Swapchain getSwapchain() override;

  void recreateSwapchain() override;

  const DeviceStats &getDeviceStats() const override { return mStats; }

  void createShader(const ShaderDescription &description,
                    ShaderHandle handle) override;

  DescriptorLayoutHandle createDescriptorLayout(
      const DescriptorLayoutDescription &description) override;

  Descriptor createDescriptor(DescriptorLayoutHandle layout) override;

  Buffer createBuffer(const BufferDescription &description) override;

  void destroyBuffer(BufferHandle handle) override;

  void createTexture(const TextureDescription &description,
                     TextureHandle handle) override;

  const TextureDescription
  getTextureDescription(TextureHandle handle) const override;

  void destroyTexture(TextureHandle handle) override;

  void createTextureView(const TextureViewDescription &description,
                         TextureHandle handle) override;

  void createSampler(const SamplerDescription &description,
                     SamplerHandle handle) override;

  void destroySampler(SamplerHandle handle) override;

  void createRenderPass(const RenderPassDescription &description,
                        RenderPassHandle handle) override;

  void destroyRenderPass(RenderPassHandle handle) override;

  void createFramebuffer(const FramebufferDescription &description,
                         FramebufferHandle handle) override;

  void destroyFramebuffer(FramebufferHandle handle) override;

  void createPipeline(const GraphicsPipelineDescription &description,
                      PipelineHandle handle) override;

  void createPipeline(const ComputePipelineDescription &description,
                      PipelineHandle handle) override;

  void destroyPipeline(PipelineHandle handle) override;

  bool hasPipeline(PipelineHandle handle) override;

private:
  VulkanRenderBackend &mBackend;
  VulkanPhysicalDevice mPhysicalDevice;
  VulkanDeviceObject mDevice;
  VulkanQueue mPresentQueue;
  VulkanQueue mGraphicsQueue;

  VulkanFrameManager mFrameManager;
  VulkanTimestampManager mTimestampManager;
  VulkanResourceAllocator mAllocator;
  VulkanResourceRegistry mRegistry;
  VulkanPipelineLayoutCache mPipelineLayoutCache;
  VulkanDescriptorPool mDescriptorPool;
  VulkanCommandPool mCommandPool;
  VulkanRenderContext mRenderContext;
  VulkanUploadContext mUploadContext;
  VulkanSwapchain mSwapchain;

  DeviceStats mStats;
};

} // namespace quoll::rhi

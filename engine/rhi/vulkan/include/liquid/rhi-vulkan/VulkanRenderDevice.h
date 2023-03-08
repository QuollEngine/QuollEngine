#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "liquid/rhi/DeviceStats.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDeviceObject.h"
#include "VulkanQueue.h"
#include "VulkanRenderContext.h"
#include "VulkanUploadContext.h"
#include "VulkanRenderBackend.h"
#include "VulkanResourceAllocator.h"
#include "VulkanResourceRegistry.h"
#include "VulkanCommandPool.h"
#include "VulkanPipelineLayoutCache.h"
#include "VulkanDescriptorPool.h"
#include "VulkanSwapchain.h"

namespace liquid::rhi {

/**
 * @brief Vulkan render device
 */
class VulkanRenderDevice : public RenderDevice {
public:
  /**
   * @brief Create Vulkan render device
   *
   * @param backend Render backend
   * @param physicalDevice Physical device
   */
  VulkanRenderDevice(VulkanRenderBackend &backend,
                     const VulkanPhysicalDevice &physicalDevice);

  /**
   * @brief Request immediate command list
   *
   * @return New command list
   */
  RenderCommandList requestImmediateCommandList() override;

  /**
   * @brief Submit commands immediately
   *
   * @param commandList Command list
   */
  void submitImmediate(RenderCommandList &commandList) override;

  /**
   * @brief Begin frame
   *
   * @return Frame index
   */
  RenderFrame beginFrame() override;

  /**
   * @brief End frame
   *
   * @param renderFrame Render frame
   */
  void endFrame(const RenderFrame &renderFrame) override;

  /**
   * @brief Wait for idle
   */
  void waitForIdle() override;

  /**
   * @brief Get physical device information
   *
   * @return Physical device information
   */
  const PhysicalDeviceInformation getDeviceInformation() override {
    return mPhysicalDevice.getDeviceInfo();
  }

  /**
   * @brief Destroy all resources in the device
   *
   * This does not destroy the device
   */
  void destroyResources() override;

  /**
   * @brief Get swapchain
   *
   * @return Swapchain
   */
  Swapchain getSwapchain() override;

  /**
   * @brief Recreate swapchain
   */
  void recreateSwapchain() override;

  /**
   * @brief Get device stats
   *
   * @return Device stats
   */
  const DeviceStats &getDeviceStats() const override { return mStats; }

  /**
   * @brief Create shader
   *
   * @param description Shader description
   * @return Shader
   */
  ShaderHandle createShader(const ShaderDescription &description) override;

  /**
   * @brief Create descriptor layout
   *
   * @param description Descriptor layout description
   * @return Descriptor layout
   */
  DescriptorLayoutHandle createDescriptorLayout(
      const DescriptorLayoutDescription &description) override;

  /**
   * @brief Create descriptor
   *
   * @param layout Descriptor layout
   * @return Descriptor
   */
  Descriptor createDescriptor(DescriptorLayoutHandle layout) override;

  /**
   * @brief Create hardware buffer
   *
   * @param description Buffer description
   * @return Buffer
   */
  Buffer createBuffer(const BufferDescription &description) override;

  /**
   * @brief Destroy buffer
   *
   * @param handle Buffer handle
   */
  void destroyBuffer(BufferHandle handle) override;

  /**
   * @brief Create texture
   *
   * @param description Texture description
   * @return Texture
   */
  TextureHandle createTexture(const TextureDescription &description) override;

  /**
   * @brief Update texture
   *
   * @param handle Texture handle
   * @param description Texture description
   */
  void updateTexture(TextureHandle handle,
                     const TextureDescription &description) override;

  /**
   * @brief Get texture description
   *
   * @param handle Texture handle
   * @return Texture description
   */
  const TextureDescription
  getTextureDescription(TextureHandle handle) const override;

  /**
   * @brief Destroy texture
   *
   * @param handle Texture handle
   */
  void destroyTexture(TextureHandle handle) override;

  /**
   * @brief Create texture view
   *
   * @param description Texture view description
   * @return Texture view
   */
  TextureViewHandle
  createTextureView(const TextureViewDescription &description) override;

  /**
   * @brief Destroy texture view
   *
   * @param handle Texture view handle
   */
  void destroyTextureView(TextureViewHandle handle) override;

  /**
   * @brief Create render pass
   *
   * @param description Render pass description
   * @return Render pass
   */
  RenderPassHandle
  createRenderPass(const RenderPassDescription &description) override;

  /**
   * @brief Destroy render pass
   *
   * @param handle Render pass handle
   */
  void destroyRenderPass(RenderPassHandle handle) override;

  /**
   * @brief Create framebuffer
   *
   * @param description Framebuffer description
   * @return Framebuffer
   */
  FramebufferHandle
  createFramebuffer(const FramebufferDescription &description) override;

  /**
   * @brief Destroy framebuffer
   *
   * @param handle Framebuffer handle
   */
  void destroyFramebuffer(FramebufferHandle handle) override;

  /**
   * @brief Create graphics pipeline
   *
   * @param description Pipeline description
   * @return Graphics pipeline
   */
  virtual PipelineHandle
  createPipeline(const GraphicsPipelineDescription &description) override;

  /**
   * @brief Create compute pipeline
   *
   * @param description Compute pipeline description
   * @return Compute pipeline
   */
  virtual PipelineHandle
  createPipeline(const ComputePipelineDescription &description) override;

  /**
   * @brief Destroy pipeline
   *
   * @param handle Pipeline handle
   */
  void destroyPipeline(PipelineHandle handle) override;

private:
  VulkanRenderBackend &mBackend;
  VulkanPhysicalDevice mPhysicalDevice;
  VulkanDeviceObject mDevice;
  VulkanQueue mPresentQueue;
  VulkanQueue mGraphicsQueue;

  VulkanFrameManager mFrameManager;
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

} // namespace liquid::rhi

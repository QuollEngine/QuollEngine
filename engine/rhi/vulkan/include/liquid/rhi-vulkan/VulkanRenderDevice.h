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
#include "VulkanDescriptorManager.h"
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
   * @brief Get texture description
   *
   * @param handle Texture handle
   * @return Texture description
   */
  const TextureDescription
  getTextureDescription(TextureHandle handle) const override;

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
   * @brief Create pipeline
   *
   * @param description Pipeline description
   * @return Pipeline
   */
  PipelineHandle
  createPipeline(const PipelineDescription &description) override;

  /**
   * @brief Destroy pipeline
   *
   * @param handle Pipeline handle
   */
  void destroyPipeline(PipelineHandle handle) override;

private:
  /**
   * @brief Recreate swapchain
   */
  void recreateSwapchain();

  /**
   * @brief Update framebuffer relative textures
   */
  void updateFramebufferRelativeTextures();

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
  VulkanDescriptorManager mDescriptorManager;
  VulkanCommandPool mCommandPool;
  VulkanRenderContext mRenderContext;
  VulkanUploadContext mUploadContext;
  VulkanSwapchain mSwapchain;

  DeviceStats mStats;

  bool mSwapchainRecreated = false;
};

} // namespace liquid::rhi

#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "liquid/rhi/ResourceRegistry.h"
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
   * @brief Synchronize resources
   *
   * @param registry Resource registry
   */
  void synchronize(ResourceRegistry &registry) override;

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
  DeviceStats mStats;

  VulkanRenderBackend &mBackend;
  VulkanPhysicalDevice mPhysicalDevice;
  VulkanDeviceObject mDevice;
  VulkanQueue mPresentQueue;
  VulkanQueue mGraphicsQueue;

  VulkanFrameManager mFrameManager;
  VulkanResourceAllocator mAllocator;
  VulkanResourceRegistry mRegistry;
  VulkanDescriptorManager mDescriptorManager;
  VulkanCommandPool mCommandPool;
  VulkanRenderContext mRenderContext;
  VulkanUploadContext mUploadContext;
  VulkanSwapchain mSwapchain;

  bool mSwapchainRecreated = false;
};

} // namespace liquid::rhi

#pragma once

#include "liquid/rhi/base/RenderDevice.h"
#include "liquid/rhi/ResourceRegistry.h"

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

namespace liquid::experimental {

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

  void synchronizeSwapchain(const VulkanSwapchain &swapchain);

  void synchronize(ResourceRegistry &registry);

  void synchronizeDeletes(ResourceRegistry &registry);

  /**
   * @brief Wait for idle
   */
  void wait();

  /**
   * @brief Get resource registry
   *
   * @return Resource registry
   */
  inline const VulkanResourceRegistry &getResourceRegistry() const {
    return mRegistry;
  }

  /**
   * @brief Get Vulkan device handle
   *
   * @return Vulkan device handle
   */
  inline VulkanDeviceObject &getVulkanDevice() { return mDevice; }

  /**
   * @brief Get physical device
   *
   * @return Physical device
   */
  inline const VulkanPhysicalDevice &getPhysicalDevice() const {
    return mPhysicalDevice;
  }

  /**
   * @brief Get backend
   *
   * @return Render backend
   */
  inline VulkanRenderBackend &getBackend() { return mBackend; }

  /**
   * @brief Get resource manager
   *
   * @return Resource manager
   */
  inline VulkanResourceAllocator &getResourceAllocator() { return mAllocator; }

  /**
   * @brief Get command pool
   *
   * @return Command pool
   */
  inline VulkanCommandPool &getCommandPool() { return mCommandPool; }

  /**
   * @brief Get render context
   *
   * @return Render context
   */
  inline VulkanRenderContext &getRenderContext() { return mRenderContext; }

private:
  VulkanRenderBackend &mBackend;
  VulkanPhysicalDevice mPhysicalDevice;
  VulkanDeviceObject mDevice;
  VulkanQueue mPresentQueue;
  VulkanQueue mGraphicsQueue;

  VulkanResourceAllocator mAllocator;
  VulkanResourceRegistry mRegistry;
  VulkanDescriptorManager mDescriptorManager;
  VulkanCommandPool mCommandPool;
  VulkanRenderContext mRenderContext;
  VulkanUploadContext mUploadContext;

  TextureHandle mNumSwapchainImages = 0;
};

} // namespace liquid::experimental

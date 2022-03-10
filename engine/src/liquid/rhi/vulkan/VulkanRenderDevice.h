#pragma once

#include "../base/RenderDevice.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDeviceObject.h"
#include "VulkanQueue.h"
#include "VulkanRenderBackend.h"
#include "VulkanResourceManager.h"
#include "VulkanResourceRegistry.h"
#include "VulkanCommandPool.h"
#include "VulkanDescriptorManager.h"
#include "VulkanRenderContext.h"
#include "VulkanUploadContext.h"

#include "../ResourceRegistry.h"

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

  /**
   * @brief Destroy render device
   */
  ~VulkanRenderDevice();

  VulkanRenderDevice(const VulkanRenderDevice &) = delete;
  VulkanRenderDevice &operator=(const VulkanRenderDevice &) = delete;
  VulkanRenderDevice(VulkanRenderDevice &&) = delete;
  VulkanRenderDevice &operator=(VulkanRenderDevice &&) = delete;

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
  inline VkDevice getVulkanDevice() const { return mDevice; }

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
  inline VulkanResourceManager &getResourceManager() { return mManager; }

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

  VulkanDescriptorManager mDescriptorManager;
  VulkanResourceRegistry mRegistry;
  VulkanCommandPool mCommandPool;
  VulkanRenderContext mRenderContext;
  VulkanUploadContext mUploadContext;
  VulkanResourceManager mManager;
};

} // namespace liquid::experimental

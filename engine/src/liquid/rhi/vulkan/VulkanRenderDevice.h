#pragma once

#include "../base/RenderDevice.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanRenderBackend.h"

namespace liquid::experimental {

class VulkanRenderDevice : public RenderDevice {
public:
  /**
   * @brief Create Vulkan render device
   *
   * @param physicalDevice Physical device
   * @param backend Render backend
   */
  VulkanRenderDevice(const VulkanPhysicalDevice &physicalDevice,
                     VulkanRenderBackend &backend);

  /**
   * @brief Destroy render device
   */
  ~VulkanRenderDevice();

  VulkanRenderDevice(const VulkanRenderDevice &) = delete;
  VulkanRenderDevice &operator=(const VulkanRenderDevice &) = delete;
  VulkanRenderDevice(VulkanRenderDevice &&) = delete;
  VulkanRenderDevice &operator=(VulkanRenderDevice &&) = delete;

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
   * @brief Get present queue
   *
   * @return Present queue
   */
  inline VkQueue getPresentQueue() const { return mPresentQueue; }

  /**
   * @brief Get graphics queue
   *
   * @return Graphics queue
   */
  inline VkQueue getGraphicsQueue() const { return mGraphicsQueue; }

  /**
   * @brief Get backend
   *
   * @return Render backend
   */
  inline VulkanRenderBackend &getBackend() { return mBackend; }

private:
  /**
   * @brief Create Vulkan device
   */
  void createVulkanDevice();

  /**
   * @brief Get device queues
   */
  void getDeviceQueues();

private:
  VkQueue mPresentQueue = VK_NULL_HANDLE;
  VkQueue mGraphicsQueue = VK_NULL_HANDLE;
  VkDevice mDevice = VK_NULL_HANDLE;

  VulkanPhysicalDevice mPhysicalDevice;
  VulkanRenderBackend &mBackend;
};

} // namespace liquid::experimental

#pragma once

#include "VulkanDeviceObject.h"

namespace liquid::rhi {

class VulkanQueue {
public:
  /**
   * @brief Create Vulkan queue
   *
   * @param device Vulkan device object
   * @param queueIndex Queue index
   */
  VulkanQueue(VulkanDeviceObject &device, uint32_t queueIndex);

  /**
   * @brief Get queue index
   *
   * @return Queue index
   */
  inline uint32_t getQueueIndex() const { return mQueueIndex; }

  /**
   * @brief Get Vulkan queue handle
   *
   * @return Vulkan queue handle
   */
  inline operator VkQueue() const { return mQueue; }

  /**
   * @brief Get Vulkan queue handle
   *
   * @return Vulkan queue handle
   */
  inline VkQueue getVulkanHandle() const { return mQueue; }

private:
  VkQueue mQueue = VK_NULL_HANDLE;
  uint32_t mQueueIndex = 0;
};

} // namespace liquid::rhi

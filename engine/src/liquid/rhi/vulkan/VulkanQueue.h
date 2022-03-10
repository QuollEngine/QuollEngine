#pragma once

#include "VulkanDeviceObject.h"

namespace liquid::experimental {

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
   * @brief Get Vulkan queue handle
   *
   * @return Vulkan queue handle
   */
  inline operator VkQueue() const { return mQueue; }

  /**
   * @brief Get queue index
   *
   * @return Queue index
   */
  inline uint32_t getQueueIndex() const { return mQueueIndex; }

private:
  VkQueue mQueue = VK_NULL_HANDLE;
  uint32_t mQueueIndex = 0;
};

} // namespace liquid::experimental

#include "liquid/core/Base.h"
#include "VulkanQueue.h"

namespace liquid::rhi {

VulkanQueue::VulkanQueue(VulkanDeviceObject &device, uint32_t queueIndex)
    : mQueueIndex(queueIndex) {
  vkGetDeviceQueue(device, mQueueIndex, 0, &mQueue);
}

} // namespace liquid::rhi

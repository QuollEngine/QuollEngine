#include "liquid/core/Base.h"
#include "VulkanQueueFamily.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanQueueFamily::VulkanQueueFamily(VkPhysicalDevice device,
                                     VkSurfaceKHR surface) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
    if (queueFamilies.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      mGraphicsFamily = i;
    }

    if (queueFamilies.at(i).queueFlags & VK_QUEUE_TRANSFER_BIT) {
      mTransferFamily = i;
    }

    VkBool32 presentSupport = false;
    checkForVulkanError(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                                             &presentSupport),
                        "Cannot query physics device surface support");

    if (presentSupport) {
      mPresentFamily = i;
    }

    if (isComplete()) {
      break;
    }
  }
}

} // namespace liquid::rhi

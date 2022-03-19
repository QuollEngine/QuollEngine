#include "liquid/core/Base.h"

#include "VulkanDeviceObject.h"
#include "VulkanError.h"

#include "liquid/core/EngineGlobals.h"

namespace liquid::rhi {

const String LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME =
    "VK_KHR_portability_subset";

VulkanDeviceObject::VulkanDeviceObject(
    const VulkanPhysicalDevice &physicalDevice) {
  float queuePriority = 1.0f;
  VkDeviceQueueCreateInfo createGraphicsQueueInfo{};
  createGraphicsQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  createGraphicsQueueInfo.flags = 0;
  createGraphicsQueueInfo.pNext = nullptr;
  createGraphicsQueueInfo.queueFamilyIndex =
      physicalDevice.getQueueFamilyIndices().getGraphicsFamily();
  createGraphicsQueueInfo.queueCount = 1;
  createGraphicsQueueInfo.pQueuePriorities = &queuePriority;

  VkDeviceQueueCreateInfo createPresentQueueInfo{};
  createPresentQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  createPresentQueueInfo.flags = 0;
  createPresentQueueInfo.pNext = nullptr;
  createPresentQueueInfo.queueFamilyIndex =
      physicalDevice.getQueueFamilyIndices().getPresentFamily();
  createPresentQueueInfo.queueCount = 1;
  createPresentQueueInfo.pQueuePriorities = &queuePriority;

  std::vector<VkDeviceQueueCreateInfo> queueInfos;
  queueInfos.push_back(createGraphicsQueueInfo);
  if (physicalDevice.getQueueFamilyIndices().getGraphicsFamily() !=
      physicalDevice.getQueueFamilyIndices().getPresentFamily()) {
  }

  const auto &pdExtensions = physicalDevice.getSupportedExtensions();

  std::vector<const char *> extensions;
  extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  LOG_DEBUG("[Vulkan] Extension enabled: " << VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  extensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
  LOG_DEBUG("[Vulkan] Extension enabled: "
            << VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

  extensions.push_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);
  LOG_DEBUG("[Vulkan] Extension enabled: "
            << VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);

  const auto &portabilityExt = std::find_if(
      pdExtensions.cbegin(), pdExtensions.cend(), [](const auto &ext) {
        return String(static_cast<const char *>(ext.extensionName)) ==
               LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
      });

  if (portabilityExt != pdExtensions.end()) {
    extensions.push_back(
        LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME.c_str());
    LOG_DEBUG("[Vulkan] Extension enabled: "
              << LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
  }

  VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptorIndexingFeatures{};
  descriptorIndexingFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
  descriptorIndexingFeatures.pNext = nullptr;
  descriptorIndexingFeatures.descriptorBindingPartiallyBound = true;

  auto &features = physicalDevice.getFeatures();

  VkDeviceCreateInfo createDeviceInfo{};
  createDeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createDeviceInfo.pNext = &descriptorIndexingFeatures;
  createDeviceInfo.flags = 0;

  createDeviceInfo.pQueueCreateInfos = queueInfos.data();
  createDeviceInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueInfos.size());
  createDeviceInfo.pEnabledFeatures = &features;
  createDeviceInfo.enabledLayerCount = 0;
  createDeviceInfo.ppEnabledLayerNames = nullptr;
  createDeviceInfo.enabledExtensionCount =
      static_cast<uint32_t>(extensions.size());
  createDeviceInfo.ppEnabledExtensionNames = extensions.data();

  checkForVulkanError(vkCreateDevice(physicalDevice.getVulkanDevice(),
                                     &createDeviceInfo, nullptr, &mDevice),
                      "Failed to create device");

  LOG_DEBUG("[Vulkan] Vulkan device created for " << physicalDevice.getName());
}

VulkanDeviceObject::~VulkanDeviceObject() {
  if (mDevice) {
    vkDestroyDevice(mDevice, nullptr);
    LOG_DEBUG("[Vulkan] Vulkan device destroyed");
  }
}

} // namespace liquid::rhi

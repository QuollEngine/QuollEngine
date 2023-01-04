#include "liquid/core/Base.h"

#include "VulkanDeviceObject.h"
#include "VulkanError.h"

#include "liquid/core/Engine.h"

namespace liquid::rhi {

const String LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME =
    "VK_KHR_portability_subset";

VulkanDeviceObject::VulkanDeviceObject(
    const VulkanPhysicalDevice &physicalDevice)
    : mPhysicalDevice(physicalDevice) {
  float queuePriority = 1.0f;
  VkDeviceQueueCreateInfo createGraphicsQueueInfo{};
  createGraphicsQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  createGraphicsQueueInfo.flags = 0;
  createGraphicsQueueInfo.pNext = nullptr;
  createGraphicsQueueInfo.queueFamilyIndex =
      physicalDevice.getQueueFamilyIndices().getGraphicsFamily();
  createGraphicsQueueInfo.queueCount = 1;
  createGraphicsQueueInfo.pQueuePriorities = &queuePriority;

  std::vector<VkDeviceQueueCreateInfo> queueInfos;
  queueInfos.push_back(createGraphicsQueueInfo);
  if (physicalDevice.getQueueFamilyIndices().getGraphicsFamily() !=
      physicalDevice.getQueueFamilyIndices().getPresentFamily()) {
    VkDeviceQueueCreateInfo createPresentQueueInfo{};
    createPresentQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    createPresentQueueInfo.flags = 0;
    createPresentQueueInfo.pNext = nullptr;
    createPresentQueueInfo.queueFamilyIndex =
        physicalDevice.getQueueFamilyIndices().getPresentFamily();
    createPresentQueueInfo.queueCount = 1;
    createPresentQueueInfo.pQueuePriorities = &queuePriority;

    // TODO: Handle multiple queue families
  }

  const auto &pdExtensions = physicalDevice.getSupportedExtensions();

  std::vector<const char *> extensions;
  extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  Engine::getLogger().info()
      << "Vulkan extension enabled: " << VK_KHR_SWAPCHAIN_EXTENSION_NAME;

  const auto &portabilityExt = std::find_if(
      pdExtensions.cbegin(), pdExtensions.cend(), [](const auto &ext) {
        return String(static_cast<const char *>(ext.extensionName)) ==
               LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
      });

  if (portabilityExt != pdExtensions.end()) {
    extensions.push_back(
        LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME.c_str());
    Engine::getLogger().info()
        << "Extension enabled: "
        << LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
  }

  VkPhysicalDeviceHostQueryResetFeatures queryResetFeatures{};
  queryResetFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES;
  queryResetFeatures.pNext = nullptr;
  queryResetFeatures.hostQueryReset = VK_TRUE;

  VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
  descriptorIndexingFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  descriptorIndexingFeatures.pNext = &queryResetFeatures;
  descriptorIndexingFeatures.descriptorBindingPartiallyBound = true;
  descriptorIndexingFeatures.runtimeDescriptorArray = true;
  descriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = true;
  descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind =
      true;
  descriptorIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind =
      true;
  descriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind;

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

  checkForVulkanError(
      vkCreateDevice(physicalDevice, &createDeviceInfo, nullptr, &mDevice),
      "Failed to create device");

  Engine::getLogger().info()
      << "Vulkan device created for " << physicalDevice.getName();
}

VulkanDeviceObject::~VulkanDeviceObject() {
  if (mDevice) {
    vkDestroyDevice(mDevice, nullptr);

    Engine::getLogger().info()
        << "Vulkan device for " << mPhysicalDevice.getName() << " destroyed";
  }
}

} // namespace liquid::rhi

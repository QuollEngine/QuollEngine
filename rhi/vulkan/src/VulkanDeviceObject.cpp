#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "VulkanDeviceObject.h"
#include "VulkanError.h"

namespace quoll::rhi {

const String QUOLL_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME =
    "VK_KHR_portability_subset";

/**
 * @brief Assert feature in debug mode or log fatal error in release
 *
 * @param featureFlag Feature flag
 * @param name Feature name
 */
static void assertFeature(VkBool32 featureFlag, String name) {
  QuollAssert(featureFlag == VK_TRUE,
              "Feature is not supported in physical device: " + name);
  if (featureFlag != VK_TRUE) {
    Engine::getLogger().fatal()
        << "Vulkan renderer requires the following feature to be enabled: "
        << name;
  }
}

VulkanDeviceObject::VulkanDeviceObject(
    const VulkanPhysicalDevice &physicalDevice)
    : mPhysicalDevice(physicalDevice) {
  f32 queuePriority = 1.0f;
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
  extensions.push_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);
  extensions.push_back(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME);
  extensions.push_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);

  const auto &portabilityExt = std::find_if(
      pdExtensions.cbegin(), pdExtensions.cend(), [](const auto &ext) {
        return String(static_cast<const char *>(ext.extensionName)) ==
               QUOLL_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
      });

  if (portabilityExt != pdExtensions.end()) {
    extensions.push_back(
        QUOLL_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME.c_str());
  }

  VkPhysicalDeviceHostQueryResetFeatures queryResetFeatures{};
  queryResetFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES;
  queryResetFeatures.pNext = nullptr;

  VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
  descriptorIndexingFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  descriptorIndexingFeatures.pNext = &queryResetFeatures;

  VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{};
  bufferDeviceAddressFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
  bufferDeviceAddressFeatures.pNext = &descriptorIndexingFeatures;

  VkPhysicalDeviceSynchronization2Features sync2Features{};
  sync2Features.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
  sync2Features.pNext = &bufferDeviceAddressFeatures;

  VkPhysicalDeviceFeatures2 deviceFeatures{};
  deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  deviceFeatures.pNext = &sync2Features;
  vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures);

  assertFeature(sync2Features.synchronization2, "Synchronization 2");
  assertFeature(bufferDeviceAddressFeatures.bufferDeviceAddress,
                "Buffer device address > Buffer device address");
  assertFeature(descriptorIndexingFeatures.descriptorBindingPartiallyBound,
                "Descriptor indexing > Partially bound descriptor binding");
  assertFeature(
      descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing,
      "Descriptor indexing > Sampled image array non-uniform indexing");
  assertFeature(
      descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind,
      "Descriptor indexing > Sampled image update after bind descriptor "
      "binding");
  assertFeature(
      descriptorIndexingFeatures.shaderUniformBufferArrayNonUniformIndexing,
      "Descriptor indexing > Uniform buffer array non-uniform indexing");
  assertFeature(
      descriptorIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind,
      "Descriptor indexing > Uniform buffer update after bind descriptor "
      "binding");
  assertFeature(
      descriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing,
      "Descriptor indexing > Storage buffer array non-uniform indexing");
  assertFeature(
      descriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind,
      "Descriptor indexing > Storage buffer update after bind descriptor "
      "binding");
  assertFeature(
      descriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind,
      "Query reset > Host query reset");

  VkDeviceCreateInfo createDeviceInfo{};
  createDeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createDeviceInfo.pNext = &deviceFeatures;
  createDeviceInfo.flags = 0;
  createDeviceInfo.pQueueCreateInfos = queueInfos.data();
  createDeviceInfo.queueCreateInfoCount = static_cast<u32>(queueInfos.size());
  createDeviceInfo.pEnabledFeatures = nullptr;
  createDeviceInfo.enabledLayerCount = 0;
  createDeviceInfo.ppEnabledLayerNames = nullptr;
  createDeviceInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
  createDeviceInfo.ppEnabledExtensionNames = extensions.data();

  checkForVulkanError(
      vkCreateDevice(physicalDevice, &createDeviceInfo, nullptr, &mDevice),
      "Failed to create device");
  volkLoadDevice(mDevice);

  for (const auto &ext : extensions) {
    Engine::getLogger().info() << "[VK] Device extension enabled: " << ext;
  }

  Engine::getLogger().info()
      << "[VK] Device created for " << physicalDevice.getName();
}

VulkanDeviceObject::~VulkanDeviceObject() {
  if (mDevice) {
    vkDestroyDevice(mDevice, nullptr);

    Engine::getLogger().info()
        << "[VK] Device for " << mPhysicalDevice.getName() << " destroyed";
  }
}

static String getObjectTypeLabel(VkObjectType type) {
  switch (type) {
  case VK_OBJECT_TYPE_BUFFER:
    return "buffer";
  case VK_OBJECT_TYPE_IMAGE:
    return "texture";
  case VK_OBJECT_TYPE_IMAGE_VIEW:
    return "texture view";
  case VK_OBJECT_TYPE_SAMPLER:
    return "sampler";
  case VK_OBJECT_TYPE_RENDER_PASS:
    return "render pass";
  case VK_OBJECT_TYPE_FRAMEBUFFER:
    return "framebuffer";
  case VK_OBJECT_TYPE_PIPELINE:
    return "pipeline";
  case VK_OBJECT_TYPE_DESCRIPTOR_SET:
    return "descriptor";
  case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
    return "descriptor layout";
  case VK_OBJECT_TYPE_SHADER_MODULE:
    return "shader";
  default:
    return "";
  };
}

void VulkanDeviceObject::setObjectName(const String &name, VkObjectType type,
                                       void *handle) {
  if (!vkSetDebugUtilsObjectNameEXT || name.empty()) {
    return;
  }

  auto objectName = name + " " + getObjectTypeLabel(type);

  VkDebugUtilsObjectNameInfoEXT nameInfo{};
  nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
  nameInfo.pNext = nullptr;
  nameInfo.pObjectName = objectName.c_str();
  nameInfo.objectType = type;
  nameInfo.objectHandle = reinterpret_cast<u64 &>(handle);

  vkSetDebugUtilsObjectNameEXT(mDevice, &nameInfo);
}

} // namespace quoll::rhi

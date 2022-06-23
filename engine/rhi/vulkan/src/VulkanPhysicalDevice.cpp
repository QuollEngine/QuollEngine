#include "liquid/core/Base.h"
#include "VulkanPhysicalDevice.h"

namespace liquid::rhi {

std::vector<VulkanPhysicalDevice>
VulkanPhysicalDevice::getPhysicalDevices(VkInstance instance,
                                         VkSurfaceKHR surface) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  std::vector<VkPhysicalDevice> rawDevices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, rawDevices.data());

  std::vector<VulkanPhysicalDevice> devices;
  for (auto &physicalDevice : rawDevices) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    VulkanQueueFamily queueFamilyIndices(physicalDevice, surface);

    devices.push_back(VulkanPhysicalDevice(physicalDevice, deviceProperties,
                                           deviceFeatures, queueFamilyIndices));
  }

  return devices;
}

VulkanPhysicalDevice::VulkanPhysicalDevice(
    const VkPhysicalDevice &device,
    const VkPhysicalDeviceProperties &properties,
    const VkPhysicalDeviceFeatures &features,
    const VulkanQueueFamily &queueFamilyIndices)
    : mDevice(device), mProperties(properties), mFeatures(features),
      mQueueFamilyIndices(queueFamilyIndices) {
  mName = String((const char *)mProperties.deviceName);
}

bool VulkanPhysicalDevice::supportsSwapchain() const {
  const auto &availableExtensions = getSupportedExtensions();

  std::vector<std::string> extensions;
  extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  std::set<std::string> requiredExtensions(extensions.begin(),
                                           extensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase((const char *)extension.extensionName);
  }

  return requiredExtensions.empty();
}

const std::vector<VkExtensionProperties>
VulkanPhysicalDevice::getSupportedExtensions() const {
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(mDevice, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(mDevice, nullptr, &extensionCount,
                                       supportedExtensions.data());

  return supportedExtensions;
}

const VkSurfaceCapabilitiesKHR VulkanPhysicalDevice::getSurfaceCapabilities(
    const VkSurfaceKHR &surface) const {
  VkSurfaceCapabilitiesKHR surfaceCapabilities{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice, surface,
                                            &surfaceCapabilities);
  return surfaceCapabilities;
}

const std::vector<VkSurfaceFormatKHR>
VulkanPhysicalDevice::getSurfaceFormats(const VkSurfaceKHR &surface) const {
  uint32_t surfaceFormatsCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice, surface, &surfaceFormatsCount,
                                       nullptr);
  std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatsCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice, surface, &surfaceFormatsCount,
                                       surfaceFormats.data());

  return surfaceFormats;
}

const std::vector<VkPresentModeKHR>
VulkanPhysicalDevice::getPresentModes(const VkSurfaceKHR &surface) const {
  uint32_t presentModesCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice, surface,
                                            &presentModesCount, nullptr);
  std::vector<VkPresentModeKHR> presentModes(presentModesCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      mDevice, surface, &presentModesCount, presentModes.data());
  return presentModes;
}

const PhysicalDeviceInformation VulkanPhysicalDevice::getDeviceInfo() const {
  VkPhysicalDeviceProperties mProperties;
  vkGetPhysicalDeviceProperties(mDevice, &mProperties);

  PhysicalDeviceType type = PhysicalDeviceType::Unknown;
  switch (mProperties.deviceType) {
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    type = PhysicalDeviceType::DiscreteGPU;
    break;
  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    type = PhysicalDeviceType::IntegratedGPU;
    break;
  case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    type = PhysicalDeviceType::VirtualGPU;
    break;
  case VK_PHYSICAL_DEVICE_TYPE_CPU:
    type = PhysicalDeviceType::CPU;
    break;
  case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
  case VK_PHYSICAL_DEVICE_TYPE_OTHER:
  default:
    type = PhysicalDeviceType::Unknown;
    break;
  }

  const auto &limits = mProperties.limits;

  PhysicalDeviceInformation::UnorderedPropertyMap propertiesMap{
      {"apiVersion", mProperties.apiVersion},
      {"driverVersion", mProperties.driverVersion},
      {"vendorID", mProperties.vendorID},
      {"deviceID", mProperties.deviceID}};

  PhysicalDeviceInformation::UnorderedPropertyMap limitsMap{
      {"maxImageDimension1D", limits.maxImageDimension1D},
      {"maxImageDimension2D", limits.maxImageDimension2D},
      {"maxImageDimension3D", limits.maxImageDimension3D},
      {"maxImageDimensionCube", limits.maxImageDimensionCube},
      {"maxImageArrayLayers", limits.maxImageArrayLayers},
      {"maxTexelBufferElements", limits.maxTexelBufferElements},
      {"maxUniformBufferRange", limits.maxUniformBufferRange},
      {"maxStorageBufferRange", limits.maxStorageBufferRange},
      {"maxPushConstantsSize", limits.maxPushConstantsSize},
      {"maxMemoryAllocationCount", limits.maxMemoryAllocationCount},
      {"maxSamplerAllocationCount", limits.maxSamplerAllocationCount},
      {"bufferImageGranularity", limits.bufferImageGranularity},
      {"sparseAddressSpaceSize", limits.sparseAddressSpaceSize},
      {"maxBoundDescriptorSets", limits.maxBoundDescriptorSets},
      {"maxPerStageDescriptorSamplers", limits.maxPerStageDescriptorSamplers},
      {"maxPerStageDescriptorUniformBuffers",
       limits.maxPerStageDescriptorUniformBuffers},
      {"maxPerStageDescriptorStorageBuffers",
       limits.maxPerStageDescriptorStorageBuffers},
      {"maxPerStageDescriptorSampledImages",
       limits.maxPerStageDescriptorSampledImages},
      {"maxPerStageDescriptorStorageImages",
       limits.maxPerStageDescriptorStorageImages},
      {"maxPerStageDescriptorInputAttachments",
       limits.maxPerStageDescriptorInputAttachments},
      {"maxPerStageResources", limits.maxPerStageResources},
      {"maxDescriptorSetSamplers", limits.maxDescriptorSetSamplers},
      {"maxDescriptorSetUniformBuffers", limits.maxDescriptorSetUniformBuffers},
      {"maxDescriptorSetUniformBuffersDynamic",
       limits.maxDescriptorSetUniformBuffersDynamic},
      {"maxDescriptorSetStorageBuffers", limits.maxDescriptorSetStorageBuffers},
      {"maxDescriptorSetStorageBuffersDynamic",
       limits.maxDescriptorSetStorageBuffersDynamic},
      {"maxDescriptorSetSampledImages", limits.maxDescriptorSetSampledImages},
      {"maxDescriptorSetStorageImages", limits.maxDescriptorSetStorageImages},
      {"maxDescriptorSetInputAttachments",
       limits.maxDescriptorSetInputAttachments},
      {"maxVertexInputAttributes", limits.maxVertexInputAttributes},
      {"maxVertexInputBindings", limits.maxVertexInputBindings},
      {"maxVertexInputAttributeOffset", limits.maxVertexInputAttributeOffset},
      {"maxVertexInputBindingStride", limits.maxVertexInputBindingStride},
      {"maxVertexOutputComponents", limits.maxVertexOutputComponents},
      {"maxTessellationGenerationLevel", limits.maxTessellationGenerationLevel},
      {"maxTessellationPatchSize", limits.maxTessellationPatchSize},
      {"maxTessellationControlPerVertexInputComponents",
       limits.maxTessellationControlPerVertexInputComponents},
      {"maxTessellationControlPerVertexOutputComponents",
       limits.maxTessellationControlPerVertexOutputComponents},
      {"maxTessellationControlPerPatchOutputComponents",
       limits.maxTessellationControlPerPatchOutputComponents},
      {"maxTessellationControlTotalOutputComponents",
       limits.maxTessellationControlTotalOutputComponents},
      {"maxTessellationEvaluationInputComponents",
       limits.maxTessellationEvaluationInputComponents},
      {"maxTessellationEvaluationOutputComponents",
       limits.maxTessellationEvaluationOutputComponents},
      {"maxGeometryShaderInvocations", limits.maxGeometryShaderInvocations},
      {"maxGeometryInputComponents", limits.maxGeometryInputComponents},
      {"maxGeometryOutputComponents", limits.maxGeometryOutputComponents},
      {"maxGeometryOutputVertices", limits.maxGeometryOutputVertices},
      {"maxGeometryTotalOutputComponents",
       limits.maxGeometryTotalOutputComponents},
      {"maxFragmentInputComponents", limits.maxFragmentInputComponents},
      {"maxFragmentOutputAttachments", limits.maxFragmentOutputAttachments},
      {"maxFragmentDualSrcAttachments", limits.maxFragmentDualSrcAttachments},
      {"maxFragmentCombinedOutputResources",
       limits.maxFragmentCombinedOutputResources},
      {"maxComputeSharedMemorySize", limits.maxComputeSharedMemorySize},
      {"maxComputeWorkGroupCount",
       glm::vec3(limits.maxComputeWorkGroupCount[0],
                 limits.maxComputeWorkGroupCount[1],
                 limits.maxComputeWorkGroupCount[2])},
      {"maxComputeWorkGroupInvocations", limits.maxComputeWorkGroupInvocations},
      {"maxComputeWorkGroupSize", glm::vec3(limits.maxComputeWorkGroupSize[0],
                                            limits.maxComputeWorkGroupSize[1],
                                            limits.maxComputeWorkGroupSize[2])},
      {"subPixelPrecisionBits", limits.subPixelPrecisionBits},
      {"subTexelPrecisionBits", limits.subTexelPrecisionBits},
      {"mipmapPrecisionBits", limits.mipmapPrecisionBits},
      {"maxDrawIndexedIndexValue", limits.maxDrawIndexedIndexValue},
      {"maxDrawIndirectCount", limits.maxDrawIndirectCount},
      {"maxSamplerLodBias", limits.maxSamplerLodBias},
      {"maxSamplerAnisotropy", limits.maxSamplerAnisotropy},
      {"maxViewports", limits.maxViewports},
      {"maxViewportDimensions", glm::vec2(limits.maxViewportDimensions[0],
                                          limits.maxViewportDimensions[1])},
      {"viewportBoundsRange",
       glm::vec2(limits.viewportBoundsRange[0], limits.viewportBoundsRange[1])},
      {"viewportSubPixelBits", limits.viewportSubPixelBits},
      {"minMemoryMapAlignment",
       static_cast<uint64_t>(limits.minMemoryMapAlignment)},
      {"minTexelBufferOffsetAlignment", limits.minTexelBufferOffsetAlignment},
      {"minUniformBufferOffsetAlignment",
       limits.minUniformBufferOffsetAlignment},
      {"minStorageBufferOffsetAlignment",
       limits.minStorageBufferOffsetAlignment},
      {"minTexelOffset", limits.minTexelOffset},
      {"maxTexelOffset", limits.maxTexelOffset},
      {"minTexelGatherOffset", limits.minTexelGatherOffset},
      {"maxTexelGatherOffset", limits.maxTexelGatherOffset},
      {"minInterpolationOffset", limits.minInterpolationOffset},
      {"maxInterpolationOffset", limits.maxInterpolationOffset},
      {"subPixelInterpolationOffsetBits",
       limits.subPixelInterpolationOffsetBits},
      {"maxFramebufferWidth", limits.maxFramebufferWidth},
      {"maxFramebufferHeight", limits.maxFramebufferHeight},
      {"maxFramebufferLayers", limits.maxFramebufferLayers},
      {"framebufferColorSampleCounts", limits.framebufferColorSampleCounts},
      {"framebufferDepthSampleCounts", limits.framebufferDepthSampleCounts},
      {"framebufferStencilSampleCounts", limits.framebufferStencilSampleCounts},
      {"framebufferNoAttachmentsSampleCounts",
       limits.framebufferNoAttachmentsSampleCounts},
      {"maxColorAttachments", limits.maxColorAttachments},
      {"sampledImageColorSampleCounts", limits.sampledImageColorSampleCounts},
      {"sampledImageIntegerSampleCounts",
       limits.sampledImageIntegerSampleCounts},
      {"sampledImageDepthSampleCounts", limits.sampledImageDepthSampleCounts},
      {"sampledImageStencilSampleCounts",
       limits.sampledImageStencilSampleCounts},
      {"storageImageSampleCounts", limits.storageImageSampleCounts},
      {"maxSampleMaskWords", limits.maxSampleMaskWords},
      {"timestampComputeAndGraphics", limits.timestampComputeAndGraphics},
      {"timestampPeriod", limits.timestampPeriod},
      {"maxClipDistances", limits.maxClipDistances},
      {"maxCullDistances", limits.maxCullDistances},
      {"maxCombinedClipAndCullDistances",
       limits.maxCombinedClipAndCullDistances},
      {"discreteQueuePriorities", limits.discreteQueuePriorities},
      {"pointSizeRange",
       glm::vec2(limits.pointSizeRange[0], limits.pointSizeRange[1])},
      {"lineWidthRange",
       glm::vec2(limits.lineWidthRange[0], limits.lineWidthRange[1])},
      {"pointSizeGranularity", limits.pointSizeGranularity},
      {"lineWidthGranularity", limits.lineWidthGranularity},
      {"strictLines", limits.strictLines},
      {"standardSampleLocations", limits.standardSampleLocations},
      {"optimalBufferCopyOffsetAlignment",
       limits.optimalBufferCopyOffsetAlignment},
      {"optimalBufferCopyRowPitchAlignment",
       limits.optimalBufferCopyRowPitchAlignment},
      {"nonCoherentAtomSize", limits.nonCoherentAtomSize}};

  return PhysicalDeviceInformation(mName, type, propertiesMap, limitsMap);
}

} // namespace liquid::rhi

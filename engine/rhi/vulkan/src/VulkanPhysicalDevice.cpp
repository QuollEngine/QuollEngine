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

    VulkanQueueFamily queueFamilyIndices(physicalDevice, surface);

    devices.push_back(VulkanPhysicalDevice(physicalDevice, deviceProperties,
                                           queueFamilyIndices));
  }

  return devices;
}

VulkanPhysicalDevice::VulkanPhysicalDevice(
    const VkPhysicalDevice &device,
    const VkPhysicalDeviceProperties &properties,
    const VulkanQueueFamily &queueFamilyIndices)
    : mDevice(device), mProperties(properties),
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
  VkPhysicalDeviceProperties vkProperties;
  vkGetPhysicalDeviceProperties(mDevice, &vkProperties);

  PhysicalDeviceType type = PhysicalDeviceType::Unknown;
  switch (vkProperties.deviceType) {
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

  const auto &vkLimits = vkProperties.limits;

  PhysicalDeviceProperties properties;
  properties.type = type;
  properties.apiName = "Vulkan";
  properties.apiVersion = vkProperties.apiVersion;
  properties.driverVersion = vkProperties.driverVersion;
  properties.deviceId = vkProperties.vendorID;
  properties.deviceId = vkProperties.deviceID;

  PhysicalDeviceLimits limits;
  limits.maxImageDimension1D = vkLimits.maxImageDimension1D;
  limits.maxImageDimension2D = vkLimits.maxImageDimension2D;
  limits.maxImageDimension3D = vkLimits.maxImageDimension3D;
  limits.maxImageDimensionCube = vkLimits.maxImageDimensionCube;
  limits.maxImageArrayLayers = vkLimits.maxImageArrayLayers;
  limits.maxTexelBufferElements = vkLimits.maxTexelBufferElements;
  limits.maxUniformBufferRange = vkLimits.maxUniformBufferRange;
  limits.maxStorageBufferRange = vkLimits.maxStorageBufferRange;
  limits.maxPushConstantsSize = vkLimits.maxPushConstantsSize;
  limits.maxMemoryAllocationCount = vkLimits.maxMemoryAllocationCount;
  limits.maxSamplerAllocationCount = vkLimits.maxSamplerAllocationCount;
  limits.bufferImageGranularity = vkLimits.bufferImageGranularity;
  limits.sparseAddressSpaceSize = vkLimits.sparseAddressSpaceSize;
  limits.maxBoundDescriptorSets = vkLimits.maxBoundDescriptorSets;
  limits.maxPerStageDescriptorSamplers = vkLimits.maxPerStageDescriptorSamplers;
  limits.maxPerStageDescriptorUniformBuffers =
      vkLimits.maxPerStageDescriptorUniformBuffers;
  limits.maxPerStageDescriptorStorageBuffers =
      vkLimits.maxPerStageDescriptorStorageBuffers;
  limits.maxPerStageDescriptorSampledImages =
      vkLimits.maxPerStageDescriptorSampledImages;
  limits.maxPerStageDescriptorStorageImages =
      vkLimits.maxPerStageDescriptorStorageImages;
  limits.maxPerStageDescriptorInputAttachments =
      vkLimits.maxPerStageDescriptorInputAttachments;
  limits.maxPerStageResources = vkLimits.maxPerStageResources;
  limits.maxDescriptorSetSamplers = vkLimits.maxDescriptorSetSamplers;
  limits.maxDescriptorSetUniformBuffers =
      vkLimits.maxDescriptorSetUniformBuffers;
  limits.maxDescriptorSetUniformBuffersDynamic =
      vkLimits.maxDescriptorSetUniformBuffersDynamic;
  limits.maxDescriptorSetStorageBuffers =
      vkLimits.maxDescriptorSetStorageBuffers;
  limits.maxDescriptorSetStorageBuffersDynamic =
      vkLimits.maxDescriptorSetStorageBuffersDynamic;
  limits.maxDescriptorSetSampledImages = vkLimits.maxDescriptorSetSampledImages;
  limits.maxDescriptorSetStorageImages = vkLimits.maxDescriptorSetStorageImages;
  limits.maxDescriptorSetInputAttachments =
      vkLimits.maxDescriptorSetInputAttachments;
  limits.maxVertexInputAttributes = vkLimits.maxVertexInputAttributes;
  limits.maxVertexInputBindings = vkLimits.maxVertexInputBindings;
  limits.maxVertexInputAttributeOffset = vkLimits.maxVertexInputAttributeOffset;
  limits.maxVertexInputBindingStride = vkLimits.maxVertexInputBindingStride;
  limits.maxVertexOutputComponents = vkLimits.maxVertexOutputComponents;
  limits.maxFragmentInputComponents = vkLimits.maxFragmentInputComponents;
  limits.maxFragmentOutputAttachments = vkLimits.maxFragmentOutputAttachments;
  limits.maxFragmentDualSrcAttachments = vkLimits.maxFragmentDualSrcAttachments;
  limits.maxFragmentCombinedOutputResources =
      vkLimits.maxFragmentCombinedOutputResources;
  limits.maxComputeSharedMemorySize = vkLimits.maxComputeSharedMemorySize;
  limits.maxComputeWorkGroupCount = {vkLimits.maxComputeWorkGroupCount[0],
                                     vkLimits.maxComputeWorkGroupCount[1],
                                     vkLimits.maxComputeWorkGroupCount[2]};
  limits.maxComputeWorkGroupInvocations =
      vkLimits.maxComputeWorkGroupInvocations;
  limits.maxComputeWorkGroupSize = {vkLimits.maxComputeWorkGroupSize[0],
                                    vkLimits.maxComputeWorkGroupSize[1],
                                    vkLimits.maxComputeWorkGroupSize[2]};
  limits.subPixelPrecisionBits = vkLimits.subPixelPrecisionBits;
  limits.subTexelPrecisionBits = vkLimits.subTexelPrecisionBits;
  limits.mipmapPrecisionBits = vkLimits.mipmapPrecisionBits;
  limits.maxDrawIndexedIndexValue = vkLimits.maxDrawIndexedIndexValue;
  limits.maxDrawIndirectCount = vkLimits.maxDrawIndirectCount;
  limits.maxSamplerLodBias = vkLimits.maxSamplerLodBias;
  limits.maxSamplerAnisotropy = vkLimits.maxSamplerAnisotropy;
  limits.maxViewports = vkLimits.maxViewports;
  limits.maxViewportDimensions = {vkLimits.maxViewportDimensions[0],
                                  vkLimits.maxViewportDimensions[1]};
  limits.viewportBoundsRange = {vkLimits.viewportBoundsRange[0],
                                vkLimits.viewportBoundsRange[1]};
  limits.viewportSubPixelBits = vkLimits.viewportSubPixelBits;
  limits.minMemoryMapAlignment = vkLimits.minMemoryMapAlignment;
  limits.minTexelBufferOffsetAlignment = vkLimits.minTexelBufferOffsetAlignment;
  limits.minUniformBufferOffsetAlignment =
      vkLimits.minUniformBufferOffsetAlignment;
  limits.minStorageBufferOffsetAlignment =
      vkLimits.minStorageBufferOffsetAlignment;
  limits.minTexelOffset = vkLimits.minTexelOffset;
  limits.maxTexelOffset = vkLimits.maxTexelOffset;
  limits.minTexelGatherOffset = vkLimits.minTexelGatherOffset;
  limits.maxTexelGatherOffset = vkLimits.maxTexelGatherOffset;
  limits.minInterpolationOffset = vkLimits.minInterpolationOffset;
  limits.maxInterpolationOffset = vkLimits.maxInterpolationOffset;
  limits.subPixelInterpolationOffsetBits =
      vkLimits.subPixelInterpolationOffsetBits;
  limits.maxFramebufferWidth = vkLimits.maxFramebufferWidth;
  limits.maxFramebufferHeight = vkLimits.maxFramebufferHeight;
  limits.maxFramebufferLayers = vkLimits.maxFramebufferLayers;
  limits.framebufferColorSampleCounts = vkLimits.framebufferColorSampleCounts;
  limits.framebufferDepthSampleCounts = vkLimits.framebufferDepthSampleCounts;
  limits.framebufferStencilSampleCounts =
      vkLimits.framebufferStencilSampleCounts;
  limits.maxColorAttachments = vkLimits.maxColorAttachments;
  limits.sampledImageColorSampleCounts = vkLimits.sampledImageColorSampleCounts;
  limits.sampledImageIntegerSampleCounts =
      vkLimits.sampledImageIntegerSampleCounts;
  limits.sampledImageDepthSampleCounts = vkLimits.sampledImageDepthSampleCounts;
  limits.sampledImageStencilSampleCounts =
      vkLimits.sampledImageStencilSampleCounts;
  limits.storageImageSampleCounts = vkLimits.storageImageSampleCounts;
  limits.maxSampleMaskWords = vkLimits.maxSampleMaskWords;
  limits.timestampComputeAndGraphics = vkLimits.timestampComputeAndGraphics;
  limits.timestampPeriod = vkLimits.timestampPeriod;
  limits.maxClipDistances = vkLimits.maxClipDistances;
  limits.maxCullDistances = limits.maxCullDistances;
  limits.maxCombinedClipAndCullDistances =
      limits.maxCombinedClipAndCullDistances;
  limits.discreteQueuePriorities = limits.discreteQueuePriorities;
  limits.pointSizeRange = {limits.pointSizeRange[0], limits.pointSizeRange[1]};
  limits.lineWidthRange = {limits.lineWidthRange[0], limits.lineWidthRange[1]};
  limits.pointSizeGranularity = limits.pointSizeGranularity;
  limits.lineWidthGranularity = limits.lineWidthGranularity;
  limits.strictLines = limits.strictLines;
  limits.standardSampleLocations = limits.standardSampleLocations;
  limits.optimalBufferCopyOffsetAlignment =
      limits.optimalBufferCopyOffsetAlignment;
  limits.optimalBufferCopyRowPitchAlignment =
      limits.optimalBufferCopyRowPitchAlignment;
  limits.nonCoherentAtomSize = limits.nonCoherentAtomSize;

  return PhysicalDeviceInformation(mName, properties, limits);
}

} // namespace liquid::rhi

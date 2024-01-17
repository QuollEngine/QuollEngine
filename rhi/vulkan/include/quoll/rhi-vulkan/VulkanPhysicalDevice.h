#pragma once

#include "quoll/rhi/PhysicalDeviceInformation.h"
#include "VulkanHeaders.h"
#include "VulkanQueueFamily.h"

namespace quoll::rhi {

class VulkanPhysicalDevice {
public:
  static std::vector<VulkanPhysicalDevice>
  getPhysicalDevices(VkInstance instance, VkSurfaceKHR surface);

public:
  VulkanPhysicalDevice() = default;

  VulkanPhysicalDevice(const VkPhysicalDevice &device,
                       const VkPhysicalDeviceProperties &properties,
                       const VulkanQueueFamily &queueFamilyIndices);

  bool supportsSwapchain() const;

  inline const String &getName() const { return mName; }

  inline const VulkanQueueFamily &getQueueFamilyIndices() const {
    return mQueueFamilyIndices;
  }

  const VkSurfaceCapabilitiesKHR
  getSurfaceCapabilities(const VkSurfaceKHR &surface) const;

  const std::vector<VkSurfaceFormatKHR>
  getSurfaceFormats(const VkSurfaceKHR &surface) const;

  const std::vector<VkPresentModeKHR>
  getPresentModes(const VkSurfaceKHR &surface) const;

  const std::vector<VkExtensionProperties> getSupportedExtensions() const;

  const PhysicalDeviceInformation getDeviceInfo() const;

  inline operator VkPhysicalDevice() const { return mDevice; }

  inline VkPhysicalDevice getVulkanHandle() const { return mDevice; }

private:
  VulkanQueueFamily mQueueFamilyIndices;
  VkPhysicalDeviceProperties mProperties{};

  String mName;

  VkPhysicalDevice mDevice = VK_NULL_HANDLE;
};

} // namespace quoll::rhi

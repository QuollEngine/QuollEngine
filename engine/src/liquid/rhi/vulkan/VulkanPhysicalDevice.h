#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanQueueFamily.h"
#include "liquid/profiler/PhysicalDeviceInformation.h"

namespace liquid::rhi {

/**
 * @brief Vulkan Physical Device information
 */
class VulkanPhysicalDevice {
public:
  /**
   * @brief Return all available physical devices
   *
   * @param instance Vulkan Instance
   * @param surface Vulkan Surface
   * @return List of vulkan physical devics
   */
  static std::vector<VulkanPhysicalDevice>
  getPhysicalDevices(VkInstance instance, VkSurfaceKHR surface);

public:
  /**
   * @brief Default Constructor
   */
  VulkanPhysicalDevice() = default;

  /**
   * @brief Constructor for Vulkan Physical Device
   *
   * @param device Vulkan Physical Device handle
   * @param properties Physical Device properties
   * @param features Physical Device features
   * @param queueFamilyIndices Queue Family Indices
   */
  VulkanPhysicalDevice(const VkPhysicalDevice &device,
                       const VkPhysicalDeviceProperties &properties,
                       const VkPhysicalDeviceFeatures &features,
                       const VulkanQueueFamily &queueFamilyIndices);

  /**
   * @brief Checks if physical device supports swapchain
   *
   * @retval true Supports swapchain
   * @retval false Does not support swapchain
   */
  bool supportsSwapchain() const;

  /**
   * @brief Gets physical device name
   *
   * @return Physical device name
   */
  inline const String &getName() const { return mName; }

  /**
   * @brief Get device features
   *
   * @return Device features
   */
  inline const VkPhysicalDeviceFeatures &getFeatures() const {
    return mFeatures;
  }

  /**
   * @brief Get queue family indices
   *
   * @return Queue family indices
   */
  inline const VulkanQueueFamily &getQueueFamilyIndices() const {
    return mQueueFamilyIndices;
  }

  /**
   * @brief Get surface capabilities
   *
   * @param surface Surface
   * @return Surface capabilities
   */
  const VkSurfaceCapabilitiesKHR
  getSurfaceCapabilities(const VkSurfaceKHR &surface) const;

  /**
   * @brief Get surface formats
   *
   * @param surface Surface
   * @return Surface formats
   */
  const std::vector<VkSurfaceFormatKHR>
  getSurfaceFormats(const VkSurfaceKHR &surface) const;

  /**
   * @brief Get present modes
   *
   * @param surface Surface
   * @return Present modes
   */
  const std::vector<VkPresentModeKHR>
  getPresentModes(const VkSurfaceKHR &surface) const;

  /**
   * @brief Gets supported extensions in device
   *
   * @return List of supported extensions
   */
  const std::vector<VkExtensionProperties> getSupportedExtensions() const;

  /**
   * @brief Get device information
   *
   * @return Physical device information
   */
  const PhysicalDeviceInformation getDeviceInfo() const;

  /**
   * @brief Gets Vulkan physical device handle
   *
   * @return Vulkan physical device handle
   */
  inline operator VkPhysicalDevice() const { return mDevice; }

  /**
   * @brief Gets Vulkan physical device handle
   *
   * @return Vulkan physical device handle
   */
  inline VkPhysicalDevice getVulkanHandle() const { return mDevice; }

private:
  VulkanQueueFamily mQueueFamilyIndices;
  VkPhysicalDeviceProperties mProperties{};
  VkPhysicalDeviceFeatures mFeatures{};

  String mName;

  VkPhysicalDevice mDevice = VK_NULL_HANDLE;
};

} // namespace liquid::rhi

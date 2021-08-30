#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanQueueFamily.h"
#include "profiler/PhysicalDeviceInformation.h"

namespace liquid {

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
   * @param surfaceCapabilities Surface Capabilities
   * @param surfaceFormats Surface Formats
   * @param presentModes Present Modes
   */
  VulkanPhysicalDevice(const VkPhysicalDevice &device,
                       const VkPhysicalDeviceProperties &properties,
                       const VkPhysicalDeviceFeatures &features,
                       const VulkanQueueFamily &queueFamilyIndices,
                       const VkSurfaceCapabilitiesKHR &surfaceCapabilities,
                       const std::vector<VkSurfaceFormatKHR> &surfaceFormats,
                       const std::vector<VkPresentModeKHR> &presentModes);

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
  inline const String &getName() const { return name; }

  /**
   * @brief Gets Vulkan device handle
   *
   * @return Vulkan device handle
   */
  inline VkPhysicalDevice getVulkanDevice() const { return device; }

  /**
   * @brief Get device features
   *
   * @return Device features
   */
  inline const VkPhysicalDeviceFeatures &getFeatures() const {
    return features;
  }

  /**
   * @brief Get queue family indices
   *
   * @return Queue family indices
   */
  inline const VulkanQueueFamily &getQueueFamilyIndices() const {
    return queueFamilyIndices;
  }

  /**
   * @brief Get surface capabilities
   *
   * @return Surface capabilities
   */
  inline const VkSurfaceCapabilitiesKHR &getSurfaceCapabilities() const {
    return surfaceCapabilities;
  }

  /**
   * @brief Get surface formats
   *
   * @return Surface formats
   */
  inline const std::vector<VkSurfaceFormatKHR> &getSurfaceFormats() const {
    return surfaceFormats;
  }

  /**
   * @brief Get present modes
   *
   * @return Present modes
   */
  inline const std::vector<VkPresentModeKHR> &getPresentModes() const {
    return presentModes;
  }

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

private:
  VulkanQueueFamily queueFamilyIndices;
  VkPhysicalDeviceProperties properties{};
  VkPhysicalDeviceFeatures features{};

  VkSurfaceCapabilitiesKHR surfaceCapabilities{};
  std::vector<VkSurfaceFormatKHR> surfaceFormats;
  std::vector<VkPresentModeKHR> presentModes;

  String name;

  VkPhysicalDevice device = nullptr;
};

} // namespace liquid

#pragma once

#include <vulkan/vulkan.hpp>
#include "core/Base.h"

#include "VulkanValidator.h"
#include "VulkanPhysicalDevice.h"

namespace liquid {

class GLFWWindow;

class VulkanContext {
public:
  static std::vector<VkExtensionProperties>
  getSupportedExtensions(const String &layerName);

public:
  /**
   * @brief Creates Vulkan context
   *
   * Creates Vulkan instance, surface, picks physical device, and
   * creates device
   *
   * @param window GLFW window
   * @param enableValidations Enable validations
   */
  VulkanContext(GLFWWindow *window, bool enableValidations);

  /**
   * @brief Destroys Vulkan context
   *
   * Destroys Vulkan device, surface, and instance
   */
  ~VulkanContext();

  VulkanContext(const VulkanContext &rhs) = delete;
  VulkanContext(VulkanContext &&rhs) = delete;
  VulkanContext &operator=(const VulkanContext &rhs) = delete;
  VulkanContext &operator=(VulkanContext &&rhs) = delete;

  /**
   * @brief Get Vulkan instance
   *
   * @return Vulkan instance
   */
  inline VkInstance getInstance() const { return instance; }

  /**
   * @brief Get surface
   *
   * @return Vulkan surface
   */
  inline VkSurfaceKHR getSurface() const { return surface; }

  /**
   * @brief Get physical device
   *
   * @return Physical device
   */
  inline const VulkanPhysicalDevice &getPhysicalDevice() const {
    return physicalDevice;
  }

  /**
   * @brief Get Vulkan device
   *
   * @return Vulkan device
   */
  inline VkDevice getDevice() const { return device; }

  /**
   * @brief Get graphics queue
   *
   * @return Graphics queue
   */
  inline VkQueue getGraphicsQueue() const { return graphicsQueue; }

  /**
   * @brief Get present queue
   *
   * @return Present queue
   */
  inline VkQueue getPresentQueue() const { return presentQueue; }

private:
  void createInstance(const String &engineName, const String &applicationName);
  void pickPhysicalDevice();
  void createLogicalDevice();

private:
  VkQueue graphicsQueue = nullptr;
  VkQueue presentQueue = nullptr;
  VkDevice device = nullptr;
  VulkanPhysicalDevice physicalDevice;

  VkSurfaceKHR surface = VK_NULL_HANDLE;

  VulkanValidator validator;
  VkInstance instance = nullptr;

  bool enableValidations;
  GLFWWindow *window;
};

} // namespace liquid

#pragma once

#include <vulkan/vulkan.hpp>

namespace liquid {

/**
 * @brief Validates vulkan
 *
 * Validates Vulkan instance and objects
 * with best practices
 */
class VulkanValidator {
public:
  /**
   * @brief Creates Vulkan debug info
   */
  VulkanValidator();

  /**
   * @brief Attaches debug info to instance info
   *
   * This function is used to enable debugging
   * Vulkan instance creation
   *
   * @param createInfo Vulkan Instance create info
   */
  void attachToInstanceCreateConfig(VkInstanceCreateInfo &createInfo);

  /**
   * @brief Attaches debug info Vulkan instance
   *
   * @param instance Vulkan instance handle
   */
  void attachToInstance(VkInstance instance);

  /**
   * @brief Detaches debug info from Vulkan instance
   *
   * @param instance Vulkan instance handle
   */
  void detachFromInstance(VkInstance instance);

private:
  /**
   * @brief Checks if validation is supported
   *
   * @retval true Validation supported
   * @retval false Validation not supported
   */
  bool checkValidationSupport();

  /**
   * @brief Creates debug utils messenger
   *
   * Loads create debug utils messenger
   * extension function and calls it
   *
   * @param instance Vulkan instance handle
   * @param pCreateInfo Pointer to debug info
   * @param pAllocator Pointer to allocation callback
   * @param[out] pDebugMessenger Pointer to Debug Messenger
   */
  static VkResult createDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkDebugUtilsMessengerEXT *pDebugMessenger);

  /**
   * @brief Destroys debug utils messenger
   *
   * Loads destroy debug utils messenger
   * extension function and calls it
   *
   * @param instance Vulkan instance handle
   * @param debugMessenger Debug messenger
   * @param pAllocator Pointer to allocation callback
   */
  static void
  destroyDebugUtilsMessengerEXT(VkInstance instance,
                                VkDebugUtilsMessengerEXT debugMessenger,
                                const VkAllocationCallbacks *pAllocator);

  /**
   * @brief Debug Callback
   *
   * Callback is called when there are validation
   * issues
   *
   * @param messageSeverity Validation message Severity
   * @param messageType Validation message type
   * @param pCallbackData Pointer to callback data
   * @param pUserData Pointer to user data
   */
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

private:
  VkInstance instance = nullptr;
  VkDebugUtilsMessengerEXT messenger = nullptr;

  std::vector<const char *> validationLayers{"VK_LAYER_KHRONOS_validation"};
  VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
};

} // namespace liquid

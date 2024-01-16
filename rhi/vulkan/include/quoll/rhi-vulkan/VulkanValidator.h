#pragma once

#include "VulkanHeaders.h"

namespace quoll::rhi {

class VulkanValidator {
public:
  VulkanValidator();

  void attachToInstanceCreateConfig(VkInstanceCreateInfo &createInfo);

  void attachToInstance(VkInstance instance);

  void detachFromInstance(VkInstance instance);

private:
  bool checkValidationSupport();

  static VkResult createDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkDebugUtilsMessengerEXT *pDebugMessenger);

  static void
  destroyDebugUtilsMessengerEXT(VkInstance instance,
                                VkDebugUtilsMessengerEXT debugMessenger,
                                const VkAllocationCallbacks *pAllocator);

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

private:
  VkDebugUtilsMessengerEXT mMessenger = VK_NULL_HANDLE;
  static constexpr std::array<const char *, 1> mValidationLayers{
      "VK_LAYER_KHRONOS_validation"};
  VkDebugUtilsMessengerCreateInfoEXT mMessengerCreateInfo{};
};

} // namespace quoll::rhi

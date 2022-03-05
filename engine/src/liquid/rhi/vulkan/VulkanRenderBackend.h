#pragma once

#include "../base/RenderBackend.h"
#include "../base/RenderDevice.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanValidator.h"

#include "liquid/window/glfw/GLFWWindow.h"

#ifdef LIQUID_DEBUG
constexpr bool ENABLE_VALIDATIONS_DEFAULT = true;
#else
constexpr bool ENABLE_VALIDATIONS_DEFAULT = false;
#endif

namespace liquid::experimental {

class VulkanRenderDevice;

class VulkanRenderBackend : public RenderBackend {
public:
  /**
   * @brief Create render backend
   *
   * Initialize vulkan instance and create surface
   * from window
   *
   * @param window Window
   * @param enableValidations Enable validations
   */
  VulkanRenderBackend(GLFWWindow &window,
                      bool enableValidations = ENABLE_VALIDATIONS_DEFAULT);

  VulkanRenderBackend(const VulkanRenderBackend &) = delete;
  VulkanRenderBackend &operator=(const VulkanRenderBackend &) = delete;
  VulkanRenderBackend(VulkanRenderBackend &&) = delete;
  VulkanRenderBackend &operator=(VulkanRenderBackend &&) = delete;

  /**
   * @brief Destroy render backend
   *
   * Destroy surface and vulkan instance
   */
  ~VulkanRenderBackend();

  /**
   * @brief Get or create device
   *
   * @return Render device
   */
  VulkanRenderDevice *getOrCreateDevice();

  /**
   * @brief Get Vulkan instance
   *
   * @return Vulkan instance
   */
  inline VkInstance getVulkanInstance() const { return mInstance; }

  /**
   * @brief Get surface
   *
   * @return Surface
   */
  inline VkSurfaceKHR getSurface() const { return mSurface; }

private:
  /**
   * @brief Create vulkan instance
   *
   * @param applicationName Application name
   * @param enableValidations Enable validations
   */
  void createInstance(const String &applicationName, bool enableValidations);

  /**
   * @brief Pick physical device
   *
   * @return Physical device
   */
  VulkanPhysicalDevice pickPhysicalDevice();

private:
  VkInstance mInstance = VK_NULL_HANDLE;
  VkSurfaceKHR mSurface = VK_NULL_HANDLE;
  VulkanValidator mValidator;
  std::unique_ptr<VulkanRenderDevice> mDevice;
};

} // namespace liquid::experimental

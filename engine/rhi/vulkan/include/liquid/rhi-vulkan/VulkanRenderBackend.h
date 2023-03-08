#pragma once

#include "liquid/rhi/RenderBackend.h"
#include "liquid/rhi/RenderDevice.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanValidator.h"

#include "liquid/window/Window.h"

namespace liquid::rhi {

class VulkanRenderDevice;

/**
 * @brief Vulkan render backend
 */
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
  VulkanRenderBackend(Window &window, bool enableValidations = true);

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
  RenderDevice *createDefaultDevice() override;

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

  /**
   * @brief Get framebuffer size
   *
   * @return Framebuffer size
   */
  inline const glm::uvec2 getFramebufferSize() const {
    return mWindow.getFramebufferSize();
  }

private:
  /**
   * @brief Create vulkan instance
   *
   * @param applicationName Application name
   * @param enableValidations Enable validations
   */
  void createInstance(StringView applicationName, bool enableValidations);

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

  Window &mWindow;
};

} // namespace liquid::rhi

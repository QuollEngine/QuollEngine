#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "VulkanContext.h"

namespace liquid {

class GLFWWindow;

/**
 * @brief Vulkan swapchain
 */
class VulkanSwapchain {
public:
  /**
   * @brief Initializes object with empty handles
   */
  VulkanSwapchain() = default;

  /**
   * @brief Creates swapchain
   *
   * @param window Pointer to window
   * @param context Vulkan context
   * @param allocator Allocator
   * @param oldSwapchain Old swapchain
   */
  VulkanSwapchain(GLFWWindow *window, const VulkanContext &context,
                  VmaAllocator allocator, VkSwapchainKHR oldSwapchain);

  /**
   * @brief Move operator for swapchain
   *
   * @param rhs Right hand side value
   */
  VulkanSwapchain &operator=(VulkanSwapchain &&rhs);

  /**
   * @brief Move constructor for swapchain
   *
   * @param rhs Right hand side value
   */
  VulkanSwapchain(VulkanSwapchain &&rhs);

  /**
   * @brief Destroys Vulkan swapchain
   */
  ~VulkanSwapchain();

  VulkanSwapchain &operator=(const VulkanSwapchain &) = delete;
  VulkanSwapchain(const VulkanSwapchain &) = delete;

  /**
   * @brief Destroys Vulkan swapchain
   */
  void destroy();

  /**
   * @brief Acquires next image and signals semaphore
   *
   * @param imageAvailableSemaphore Semaphore to signal
   * @return Next acquired image index
   */
  uint32_t acquireNextImage(VkSemaphore imageAvailableSemaphore);

  /**
   * @brief Gets surface format
   *
   * @return Surface format
   */
  inline VkSurfaceFormatKHR getSurfaceFormat() { return surfaceFormat; }

  /**
   * @brief Gets present mode
   *
   * @return Present mode
   */
  inline VkPresentModeKHR getPresentMode() { return presentMode; }

  /**
   * @brief Gets Vulkan swapchain
   *
   * @return Vulkan swapchain handle
   */
  inline VkSwapchainKHR getSwapchain() const { return swapchain; }

  /**
   * @brief Gets extent
   *
   * @return Vulkan extent
   */
  inline const VkExtent2D &getExtent() { return extent; }

  /**
   * @brief Gets Vulkan image views
   *
   * @return Vulkan image views
   */
  inline const std::vector<VkImageView> &getImageViews() const {
    return imageViews;
  }

private:
  /**
   * @brief Picks most suitable surface format
   *
   * @param surfaceFormats List of surface formats
   */
  void pickMostSuitableSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &surfaceFormats);

  /**
   * @brief Picks most present mode
   *
   * @param presentModes List of present modes
   */
  void pickMostSuitablePresentMode(
      const std::vector<VkPresentModeKHR> &presentModes);

  /**
   * @brief Calculates extent from surface capabilities and window
   *
   * @param capabilities Surface capabilities
   * @param window Pointer to window
   */
  void calculateExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                       GLFWWindow *window);

  /**
   * @brief Get suitable composite alpha
   *
   * @param surfaceCapabilities Surface capabilities
   * @return Composite alpha
   */
  VkCompositeAlphaFlagBitsKHR
  getSuitableCompositeAlpha(const VkSurfaceCapabilitiesKHR &capabilities) const;

private:
  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  std::vector<VkImageView> imageViews;

  VmaAllocator allocator = nullptr;

  VkExtent2D extent{};
  VkSurfaceFormatKHR surfaceFormat{};
  VkPresentModeKHR presentMode{};

  GLFWWindow *window = nullptr;
  VkDevice device = nullptr;
};

} // namespace liquid

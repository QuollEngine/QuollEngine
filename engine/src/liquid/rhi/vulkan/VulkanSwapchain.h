#pragma once

#include <vulkan/vulkan.hpp>

#include "VulkanRenderBackend.h"
#include "VulkanDeviceObject.h"
#include "VulkanPhysicalDevice.h"

namespace liquid::experimental {

/**
 * @brief Vulkan swapchain
 */
class VulkanSwapchain {
public:
  /**
   * @brief Creates swapchain
   *
   * @param backend Vulkan backend
   * @param physicalDevice Physical device
   * @param device Vulkan device object
   * @param size Size object
   * @param oldSwapchain Old swapchain
   */
  VulkanSwapchain(const VulkanRenderBackend &backend,
                  const VulkanPhysicalDevice &physicalDevice,
                  VulkanDeviceObject &device, const glm::uvec2 &size,
                  VkSwapchainKHR oldSwapchain);

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
  inline VkSurfaceFormatKHR getSurfaceFormat() { return mSurfaceFormat; }

  /**
   * @brief Gets present mode
   *
   * @return Present mode
   */
  inline VkPresentModeKHR getPresentMode() { return mPresentMode; }

  /**
   * @brief Gets Vulkan swapchain
   *
   * @return Vulkan swapchain handle
   */
  inline VkSwapchainKHR getSwapchain() const { return mSwapchain; }

  /**
   * @brief Gets extent
   *
   * @return Vulkan extent
   */
  inline const VkExtent2D &getExtent() { return mExtent; }

  /**
   * @brief Gets Vulkan image views
   *
   * @return Vulkan image views
   */
  inline const std::vector<VkImageView> &getImageViews() const {
    return mImageViews;
  }

private:
  /**
   * @brief Destroys Vulkan swapchain
   */
  void destroy();

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
   * @param presentModes List of pGresent modes
   */
  void pickMostSuitablePresentMode(
      const std::vector<VkPresentModeKHR> &presentModes);

  /**
   * @brief Calculates extent from surface capabilities and window
   *
   * @param capabilities Surface capabilities
   * @param size Window size object
   */
  void calculateExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                       const glm::uvec2 &size);

  /**
   * @brief Get suitable composite alpha
   *
   * @param surfaceCapabilities Surface capabilities
   * @return Composite alpha
   */
  VkCompositeAlphaFlagBitsKHR
  getSuitableCompositeAlpha(const VkSurfaceCapabilitiesKHR &capabilities) const;

private:
  VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
  std::vector<VkImageView> mImageViews;

  VkExtent2D mExtent{};
  VkSurfaceFormatKHR mSurfaceFormat{};
  VkPresentModeKHR mPresentMode{};

  VulkanDeviceObject &mDevice;
};

} // namespace liquid::experimental

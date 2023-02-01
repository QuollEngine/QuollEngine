#pragma once

#include "VulkanHeaders.h"

#include "VulkanRenderBackend.h"
#include "VulkanDeviceObject.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanResourceRegistry.h"
#include "VulkanTexture.h"

namespace liquid::rhi {

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
   * @param registry Vulkan resource registry
   * @param allocator Vulkan resource allocator
   */
  VulkanSwapchain(VulkanRenderBackend &backend,
                  const VulkanPhysicalDevice &physicalDevice,
                  VulkanDeviceObject &device, VulkanResourceRegistry &registry,
                  VulkanResourceAllocator &allocator);

  /**
   * @brief Destroys Vulkan swapchain
   */
  ~VulkanSwapchain();

  VulkanSwapchain &operator=(const VulkanSwapchain &) = delete;
  VulkanSwapchain(const VulkanSwapchain &) = delete;
  VulkanSwapchain &operator=(VulkanSwapchain &&rhs) = delete;
  VulkanSwapchain(VulkanSwapchain &&rhs) = delete;

  /**
   * @brief Recreate swapchain
   *
   * @param backend Vulkan backend
   * @param physicalDevice Physical device
   * @param allocator Vulkan resource allocator
   */
  void recreate(VulkanRenderBackend &backend,
                const VulkanPhysicalDevice &physicalDevice,
                VulkanResourceAllocator &allocator);

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
  inline VkSurfaceFormatKHR getSurfaceFormat() const { return mSurfaceFormat; }

  /**
   * @brief Gets present mode
   *
   * @return Present mode
   */
  inline VkPresentModeKHR getPresentMode() const { return mPresentMode; }

  /**
   * @brief Gets extent
   *
   * @return Vulkan extent
   */
  inline const glm::uvec2 &getExtent() { return mExtent; }

  /**
   * @brief Get swapchain textures
   *
   * @return Swapchain textures
   */
  inline const std::vector<TextureHandle> &getTextures() const {
    return mTextures;
  }

  /**
   * @brief Gets Vulkan swapchain handle
   *
   * @return Vulkan swapchain handle
   */
  inline operator VkSwapchainKHR() const { return mSwapchain; }

  /**
   * @brief Gets Vulkan swapchain handle
   *
   * @return Vulkan swapchain handle
   */
  inline VkSwapchainKHR getVulkanHandle() const { return mSwapchain; }

private:
  /**
   * @brief Destroys Vulkan swapchain
   */
  void destroy();

  /**
   * @brief Create swapchain
   *
   * @param backend Vulkan backend
   * @param physicalDevice Physical device
   * @param allocator Vulkan resource allocator
   */
  void create(VulkanRenderBackend &backend,
              const VulkanPhysicalDevice &physicalDevice,
              VulkanResourceAllocator &allocator);

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
  std::vector<TextureHandle> mTextures;

  glm::uvec2 mExtent{};
  VkSurfaceFormatKHR mSurfaceFormat{};
  VkPresentModeKHR mPresentMode{};

  VulkanResourceRegistry &mRegistry;
  VulkanDeviceObject &mDevice;
};

} // namespace liquid::rhi

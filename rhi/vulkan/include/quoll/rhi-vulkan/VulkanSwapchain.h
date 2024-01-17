#pragma once

#include "VulkanDeviceObject.h"
#include "VulkanHeaders.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanRenderBackend.h"
#include "VulkanResourceRegistry.h"
#include "VulkanTexture.h"

namespace quoll::rhi {

class VulkanSwapchain {
public:
  VulkanSwapchain(VulkanRenderBackend &backend,
                  const VulkanPhysicalDevice &physicalDevice,
                  VulkanDeviceObject &device, VulkanResourceRegistry &registry,
                  VulkanResourceAllocator &allocator);

  ~VulkanSwapchain();

  VulkanSwapchain &operator=(const VulkanSwapchain &) = delete;
  VulkanSwapchain(const VulkanSwapchain &) = delete;
  VulkanSwapchain &operator=(VulkanSwapchain &&rhs) = delete;
  VulkanSwapchain(VulkanSwapchain &&rhs) = delete;

  void recreate(VulkanRenderBackend &backend,
                const VulkanPhysicalDevice &physicalDevice,
                VulkanResourceAllocator &allocator);

  u32 acquireNextImage(VkSemaphore imageAvailableSemaphore);

  inline VkSurfaceFormatKHR getSurfaceFormat() const { return mSurfaceFormat; }

  inline VkPresentModeKHR getPresentMode() const { return mPresentMode; }

  inline const glm::uvec2 &getExtent() { return mExtent; }

  inline const std::vector<TextureHandle> &getTextures() const {
    return mTextures;
  }

  inline operator VkSwapchainKHR() const { return mSwapchain; }

  inline VkSwapchainKHR getVulkanHandle() const { return mSwapchain; }

private:
  void destroy();

  void create(VulkanRenderBackend &backend,
              const VulkanPhysicalDevice &physicalDevice,
              VulkanResourceAllocator &allocator);

  void pickMostSuitableSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &surfaceFormats);

  void pickMostSuitablePresentMode(
      const std::vector<VkPresentModeKHR> &presentModes);

  void calculateExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                       const glm::uvec2 &size);

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

} // namespace quoll::rhi

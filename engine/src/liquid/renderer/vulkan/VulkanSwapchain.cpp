#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/window/glfw/GLFWWindow.h"
#include "liquid/rhi/vulkan/VulkanPhysicalDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanError.h"

namespace liquid {
//
// std::vector<VkExtensionProperties>
// VulkanContext::getSupportedExtensions(const String &layerName) {
//  uint32_t supportedExtensionCount = 0;
//
//  vkEnumerateInstanceExtensionProperties(layerName.c_str(),
//                                         &supportedExtensionCount, nullptr);
//
//  std::vector<VkExtensionProperties> supportedExtensions(
//      supportedExtensionCount);
//  vkEnumerateInstanceExtensionProperties(
//      layerName.c_str(), &supportedExtensionCount,
//      supportedExtensions.data());
//
//  return supportedExtensions;
//}

VulkanSwapchain::VulkanSwapchain(GLFWWindow *window_,
                                 experimental::VulkanRenderDevice *device_,
                                 VkSwapchainKHR oldSwapchain)
    : window(window_), device(device_),
      allocator(device->getResourceManager().getVmaAllocator()) {
  const auto &surfaceCapabilities =
      device->getPhysicalDevice().getSurfaceCapabilities(
          device->getBackend().getSurface());

  pickMostSuitableSurfaceFormat(device->getPhysicalDevice().getSurfaceFormats(
      device->getBackend().getSurface()));
  pickMostSuitablePresentMode(device->getPhysicalDevice().getPresentModes(
      device->getBackend().getSurface()));
  calculateExtent(surfaceCapabilities, window);

  uint32_t imageCount = std::min(surfaceCapabilities.minImageCount + 1,
                                 surfaceCapabilities.maxImageCount);

  bool sameQueueFamily =
      device->getPhysicalDevice()
          .getQueueFamilyIndices()
          .graphicsFamily.value() ==
      device->getPhysicalDevice().getQueueFamilyIndices().presentFamily.value();

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.surface = device->getBackend().getSurface();
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  if (sameQueueFamily) {
    auto &array = device->getPhysicalDevice().getQueueFamilyIndices().toArray();
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = static_cast<uint32_t>(array.size());
    createInfo.pQueueFamilyIndices = array.data();
  } else {
    // TODO: Handle the case where graphics
    // and present queues are not the same
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  createInfo.preTransform = surfaceCapabilities.currentTransform;
  createInfo.compositeAlpha = getSuitableCompositeAlpha(surfaceCapabilities);
  createInfo.presentMode = presentMode;
  createInfo.clipped = true;
  createInfo.oldSwapchain = oldSwapchain;

  checkForVulkanError(vkCreateSwapchainKHR(device->getVulkanDevice(),
                                           &createInfo, nullptr, &swapchain),
                      "Failed to create swapchain");

  LOG_DEBUG("[Vulkan] Swapchain created");

  vkGetSwapchainImagesKHR(device->getVulkanDevice(), swapchain, &imageCount,
                          nullptr);
  std::vector<VkImage> swapchainImages(imageCount);
  vkGetSwapchainImagesKHR(device->getVulkanDevice(), swapchain, &imageCount,
                          swapchainImages.data());

  imageViews.resize(swapchainImages.size());

  LOG_DEBUG("[Vulkan] Found swapchain images: " << swapchainImages.size());

  for (size_t i = 0; i < swapchainImages.size(); ++i) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapchainImages[i];

    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = surfaceFormat.format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;

    checkForVulkanError(vkCreateImageView(device->getVulkanDevice(),
                                          &createInfo, nullptr, &imageViews[i]),
                        "Failed to create image views for swapchain");
  }

  LOG_DEBUG("[Vulkan] Swapchain image views created");
}

VulkanSwapchain::VulkanSwapchain(VulkanSwapchain &&rhs) {
  destroy();

  swapchain = rhs.swapchain;
  imageViews = rhs.imageViews;

  allocator = rhs.allocator;

  extent = rhs.extent;
  surfaceFormat = rhs.surfaceFormat;
  presentMode = rhs.presentMode;

  window = rhs.window;
  device = rhs.device;

  rhs.swapchain = VK_NULL_HANDLE;
  rhs.imageViews.clear();
}

VulkanSwapchain &VulkanSwapchain::operator=(VulkanSwapchain &&rhs) {
  // Destroy existing swapchain
  // before moving other swapchain data
  // to this one
  destroy();

  swapchain = rhs.swapchain;
  imageViews = rhs.imageViews;

  allocator = rhs.allocator;

  extent = rhs.extent;
  surfaceFormat = rhs.surfaceFormat;
  presentMode = rhs.presentMode;

  window = rhs.window;
  device = rhs.device;

  rhs.swapchain = VK_NULL_HANDLE;
  rhs.imageViews.clear();

  return *this;
}

VulkanSwapchain::~VulkanSwapchain() { destroy(); }

void VulkanSwapchain::destroy() {
  if (!imageViews.empty()) {
    for (auto &x : imageViews) {
      vkDestroyImageView(device->getVulkanDevice(), x, nullptr);
    }
    imageViews.clear();
    LOG_DEBUG("[Vulkan] Swapchain image views destroyed");
  }

  if (swapchain) {
    vkDestroySwapchainKHR(device->getVulkanDevice(), swapchain, nullptr);
    swapchain = VK_NULL_HANDLE;
    LOG_DEBUG("[Vulkan] Swapchain destroyed");
  }
}

void VulkanSwapchain::pickMostSuitableSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &surfaceFormats) {

  auto it = std::find_if(
      surfaceFormats.begin(), surfaceFormats.end(),
      [](auto surfaceFormat) -> bool {
        return surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
               surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
      });

  surfaceFormat = it != surfaceFormats.end() ? *it : surfaceFormats[0];
}

void VulkanSwapchain::pickMostSuitablePresentMode(
    const std::vector<VkPresentModeKHR> &presentModes) {

  auto it = std::find_if(presentModes.begin(), presentModes.end(),
                         [](auto presentMode) {
                           return presentMode == VK_PRESENT_MODE_MAILBOX_KHR;
                         });

  presentMode = it != presentModes.end() ? *it : VK_PRESENT_MODE_FIFO_KHR;
}

void VulkanSwapchain::calculateExtent(
    const VkSurfaceCapabilitiesKHR &capabilities, GLFWWindow *window) {
  auto size = window->getFramebufferSize();

  uint32_t width =
      std::max(capabilities.minImageExtent.width,
               std::min(capabilities.maxImageExtent.width, size.width));
  uint32_t height =
      std::max(capabilities.minImageExtent.height,
               std::min(capabilities.maxImageExtent.height, size.height));

  extent = VkExtent2D{width, height};
}

VkCompositeAlphaFlagBitsKHR VulkanSwapchain::getSuitableCompositeAlpha(
    const VkSurfaceCapabilitiesKHR &capabilities) const {
  if (capabilities.supportedCompositeAlpha &
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
    return VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
  }

  if (capabilities.supportedCompositeAlpha &
      VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
    return VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
  }

  if (capabilities.supportedCompositeAlpha &
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
    return VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
  }

  return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
}

uint32_t
VulkanSwapchain::acquireNextImage(VkSemaphore imageAvailableSemaphore) {
  LIQUID_PROFILE_EVENT("VulkanSwapchain::acquireNextImage");
  uint32_t imageIndex = 0;
  VkResult result = vkAcquireNextImageKHR(device->getVulkanDevice(), swapchain,
                                          std::numeric_limits<uint64_t>::max(),
                                          imageAvailableSemaphore,
                                          VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    return std::numeric_limits<uint32_t>::max();
  }

  return imageIndex;
}

} // namespace liquid

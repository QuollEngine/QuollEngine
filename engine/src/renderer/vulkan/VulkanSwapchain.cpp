#include "core/Base.h"
#include "core/EngineGlobals.h"
#include "window/glfw/GLFWWindow.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanError.h"

namespace liquid {

std::vector<VkExtensionProperties>
VulkanContext::getSupportedExtensions(const String &layerName) {
  uint32_t supportedExtensionCount = 0;

  vkEnumerateInstanceExtensionProperties(layerName.c_str(),
                                         &supportedExtensionCount, nullptr);

  std::vector<VkExtensionProperties> supportedExtensions(
      supportedExtensionCount);
  vkEnumerateInstanceExtensionProperties(
      layerName.c_str(), &supportedExtensionCount, supportedExtensions.data());

  return supportedExtensions;
}

VulkanSwapchain::VulkanSwapchain(GLFWWindow *window_,
                                 const VulkanContext &context,
                                 VmaAllocator allocator_,
                                 VkSwapchainKHR oldSwapchain)
    : window(window_), device(context.getDevice()), allocator(allocator_) {
  const auto &surfaceCapabilities =
      context.getPhysicalDevice().getSurfaceCapabilities(context.getSurface());

  pickMostSuitableSurfaceFormat(
      context.getPhysicalDevice().getSurfaceFormats(context.getSurface()));
  pickMostSuitablePresentMode(
      context.getPhysicalDevice().getPresentModes(context.getSurface()));
  calculateExtent(surfaceCapabilities, window);

  uint32_t imageCount = std::min(surfaceCapabilities.minImageCount + 1,
                                 surfaceCapabilities.maxImageCount);

  bool sameQueueFamily =
      context.getPhysicalDevice()
          .getQueueFamilyIndices()
          .graphicsFamily.value() ==
      context.getPhysicalDevice().getQueueFamilyIndices().presentFamily.value();

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.surface = context.getSurface();
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  if (sameQueueFamily) {
    auto &array = context.getPhysicalDevice().getQueueFamilyIndices().toArray();
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

  checkForVulkanError(
      vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain),
      "Failed to create swapchain");

  LOG_DEBUG("[Vulkan] Swapchain created");

  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
  std::vector<VkImage> swapchainImages(imageCount);
  vkGetSwapchainImagesKHR(device, swapchain, &imageCount,
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

    checkForVulkanError(
        vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]),
        "Failed to create image views for swapchain");
  }

  LOG_DEBUG("[Vulkan] Swapchain image views created");

  VkExtent3D depthImageExtent = {extent.width, extent.height, 1};

  VkImageCreateInfo depthImageCreateInfo = {};
  depthImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  depthImageCreateInfo.pNext = nullptr;
  depthImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  depthImageCreateInfo.format = depthFormat;
  depthImageCreateInfo.extent = depthImageExtent;
  depthImageCreateInfo.mipLevels = 1;
  depthImageCreateInfo.arrayLayers = 1;
  depthImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  depthImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  depthImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

  VmaAllocationCreateInfo depthImageAllocationCreateInfo{};
  depthImageAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  depthImageAllocationCreateInfo.requiredFlags =
      VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  checkForVulkanError(vmaCreateImage(allocator_, &depthImageCreateInfo,
                                     &depthImageAllocationCreateInfo,
                                     &depthImage, &depthImageAllocation,
                                     nullptr),
                      "Depth image creation failed");

  VkImageViewCreateInfo imageViewCreateInfo = {};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.pNext = nullptr;

  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.image = depthImage;
  imageViewCreateInfo.format = depthFormat;
  imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
  imageViewCreateInfo.subresourceRange.levelCount = 1;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
  imageViewCreateInfo.subresourceRange.layerCount = 1;
  imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

  checkForVulkanError(
      vkCreateImageView(device, &imageViewCreateInfo, nullptr, &depthImageView),
      "Depth image view creation failed");

  LOG_DEBUG("[Vulkan] Depth image view created");
}

VulkanSwapchain::VulkanSwapchain(VulkanSwapchain &&rhs) {
  destroy();

  swapchain = rhs.swapchain;
  imageViews = rhs.imageViews;

  depthImageView = rhs.depthImageView;
  depthImage = rhs.depthImage;
  depthImageAllocation = rhs.depthImageAllocation;
  allocator = rhs.allocator;
  depthFormat = rhs.depthFormat;

  extent = rhs.extent;
  surfaceFormat = rhs.surfaceFormat;
  presentMode = rhs.presentMode;

  window = rhs.window;
  device = rhs.device;

  rhs.swapchain = VK_NULL_HANDLE;
  rhs.imageViews.clear();
  rhs.depthImageView = VK_NULL_HANDLE;
  rhs.depthImage = VK_NULL_HANDLE;
}

VulkanSwapchain &VulkanSwapchain::operator=(VulkanSwapchain &&rhs) {
  // Destroy existing swapchain
  // before moving other swapchain data
  // to this one
  destroy();

  swapchain = rhs.swapchain;
  imageViews = rhs.imageViews;

  depthImageView = rhs.depthImageView;
  depthImage = rhs.depthImage;
  depthImageAllocation = rhs.depthImageAllocation;
  allocator = rhs.allocator;
  depthFormat = rhs.depthFormat;

  extent = rhs.extent;
  surfaceFormat = rhs.surfaceFormat;
  presentMode = rhs.presentMode;

  window = rhs.window;
  device = rhs.device;

  rhs.swapchain = VK_NULL_HANDLE;
  rhs.imageViews.clear();
  rhs.depthImageView = VK_NULL_HANDLE;
  rhs.depthImage = VK_NULL_HANDLE;

  return *this;
}

VulkanSwapchain::~VulkanSwapchain() { destroy(); }

void VulkanSwapchain::destroy() {
  if (depthImageView) {
    vkDestroyImageView(device, depthImageView, nullptr);
    depthImageView = VK_NULL_HANDLE;
    LOG_DEBUG("[Vulkan] Depth image view destroyed");
  }

  if (depthImage) {
    vmaDestroyImage(allocator, depthImage, depthImageAllocation);
    depthImage = VK_NULL_HANDLE;
    LOG_DEBUG("[Vulkan] Depth image destroyed");
  }

  if (!imageViews.empty()) {
    for (auto &x : imageViews) {
      vkDestroyImageView(device, x, nullptr);
    }
    imageViews.clear();
    LOG_DEBUG("[Vulkan] Swapchain image views destroyed");
  }

  if (swapchain) {
    vkDestroySwapchainKHR(device, swapchain, nullptr);
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
  uint32_t imageIndex = 0;
  VkResult result = vkAcquireNextImageKHR(
      device, swapchain, std::numeric_limits<uint64_t>::max(),
      imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    return std::numeric_limits<uint32_t>::max();
  }

  return imageIndex;
}

} // namespace liquid

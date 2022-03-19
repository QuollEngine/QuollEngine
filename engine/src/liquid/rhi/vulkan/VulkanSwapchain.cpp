#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

#include "VulkanSwapchain.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanSwapchain::VulkanSwapchain(VulkanRenderBackend &backend,
                                 const VulkanPhysicalDevice &physicalDevice,
                                 VulkanDeviceObject &device,
                                 VkSwapchainKHR oldSwapchain)
    : mDevice(device) {
  const auto &surfaceCapabilities =
      physicalDevice.getSurfaceCapabilities(backend.getSurface());

  pickMostSuitableSurfaceFormat(
      physicalDevice.getSurfaceFormats(backend.getSurface()));
  pickMostSuitablePresentMode(
      physicalDevice.getPresentModes(backend.getSurface()));
  calculateExtent(surfaceCapabilities, backend.getFramebufferSize());

  uint32_t imageCount = std::min(surfaceCapabilities.minImageCount + 1,
                                 surfaceCapabilities.maxImageCount);

  bool sameQueueFamily =
      physicalDevice.getQueueFamilyIndices().getGraphicsFamily() ==
      physicalDevice.getQueueFamilyIndices().getPresentFamily();

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.surface = backend.getSurface();
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = mSurfaceFormat.format;
  createInfo.imageColorSpace = mSurfaceFormat.colorSpace;
  createInfo.imageExtent = VkExtent2D{mExtent.x, mExtent.y};
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  if (sameQueueFamily) {
    auto &array = physicalDevice.getQueueFamilyIndices().toArray();
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
  createInfo.presentMode = mPresentMode;
  createInfo.clipped = true;
  createInfo.oldSwapchain = oldSwapchain;

  checkForVulkanError(
      vkCreateSwapchainKHR(device, &createInfo, nullptr, &mSwapchain),
      "Failed to create swapchain");

  vkGetSwapchainImagesKHR(device, mSwapchain, &imageCount, nullptr);
  mImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, mSwapchain, &imageCount, mImages.data());

  mImageViews.resize(mImages.size());

  for (size_t i = 0; i < mImages.size(); ++i) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = mImages.at(i);

    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = mSurfaceFormat.format;
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
        vkCreateImageView(device, &createInfo, nullptr, &mImageViews.at(i)),
        "Failed to create image views for swapchain");
  }

  LOG_DEBUG("[Vulkan] Swapchain created (" << mImages.size() << " images)");
}

VulkanSwapchain::VulkanSwapchain(VulkanSwapchain &&rhs) : mDevice(rhs.mDevice) {
  destroy();

  mSwapchain = rhs.mSwapchain;
  mImageViews = rhs.mImageViews;
  mImages = rhs.mImages;

  mExtent = rhs.mExtent;
  mSurfaceFormat = rhs.mSurfaceFormat;
  mPresentMode = rhs.mPresentMode;

  rhs.mSwapchain = VK_NULL_HANDLE;
  rhs.mImageViews.clear();
  rhs.mImages.clear();
}

VulkanSwapchain &VulkanSwapchain::operator=(VulkanSwapchain &&rhs) {
  // Destroy existing swapchain
  // before moving other swapchain data
  // to this one
  destroy();

  mSwapchain = rhs.mSwapchain;
  mImageViews = rhs.mImageViews;

  mExtent = rhs.mExtent;
  mSurfaceFormat = rhs.mSurfaceFormat;
  mPresentMode = rhs.mPresentMode;

  rhs.mSwapchain = VK_NULL_HANDLE;
  rhs.mImageViews.clear();

  return *this;
}

VulkanSwapchain::~VulkanSwapchain() { destroy(); }

void VulkanSwapchain::destroy() {
  if (mSwapchain) {
    vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
    mSwapchain = VK_NULL_HANDLE;
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

  mSurfaceFormat = it != surfaceFormats.end() ? *it : surfaceFormats[0];
}

void VulkanSwapchain::pickMostSuitablePresentMode(
    const std::vector<VkPresentModeKHR> &presentModes) {

  auto it = std::find_if(presentModes.begin(), presentModes.end(),
                         [](auto presentMode) {
                           return presentMode == VK_PRESENT_MODE_MAILBOX_KHR;
                         });

  mPresentMode = it != presentModes.end() ? *it : VK_PRESENT_MODE_FIFO_KHR;
}

void VulkanSwapchain::calculateExtent(
    const VkSurfaceCapabilitiesKHR &capabilities, const glm::uvec2 &size) {

  uint32_t width =
      std::max(capabilities.minImageExtent.width,
               std::min(capabilities.maxImageExtent.width, size.x));
  uint32_t height =
      std::max(capabilities.minImageExtent.height,
               std::min(capabilities.maxImageExtent.height, size.y));

  mExtent = glm::uvec2{width, height};
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
  VkResult result = vkAcquireNextImageKHR(
      mDevice, mSwapchain, std::numeric_limits<uint64_t>::max(),
      imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    return std::numeric_limits<uint32_t>::max();
  }

  return imageIndex;
}

} // namespace liquid::rhi

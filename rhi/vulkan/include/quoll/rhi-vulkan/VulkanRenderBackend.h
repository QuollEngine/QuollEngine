#pragma once

#include "quoll/rhi/RenderBackend.h"
#include "quoll/rhi/RenderDevice.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanValidator.h"

#include "quoll/window/Window.h"

namespace quoll::rhi {

class VulkanRenderDevice;

class VulkanRenderBackend : public RenderBackend {
public:
  VulkanRenderBackend(Window &window, bool enableValidations = true);

  VulkanRenderBackend(const VulkanRenderBackend &) = delete;
  VulkanRenderBackend &operator=(const VulkanRenderBackend &) = delete;
  VulkanRenderBackend(VulkanRenderBackend &&) = delete;
  VulkanRenderBackend &operator=(VulkanRenderBackend &&) = delete;

  ~VulkanRenderBackend();

  RenderDevice *createDefaultDevice() override;

  inline VkInstance getVulkanInstance() const { return mInstance; }

  inline VkSurfaceKHR getSurface() const { return mSurface; }

  inline const glm::uvec2 getFramebufferSize() const {
    return mWindow.getFramebufferSize();
  }

private:
  void createInstance(StringView applicationName, bool enableValidations);

  VulkanPhysicalDevice pickPhysicalDevice();

private:
  VkInstance mInstance = VK_NULL_HANDLE;
  VkSurfaceKHR mSurface = VK_NULL_HANDLE;
  VulkanValidator mValidator;
  std::unique_ptr<VulkanRenderDevice> mDevice;

  Window &mWindow;
};

} // namespace quoll::rhi

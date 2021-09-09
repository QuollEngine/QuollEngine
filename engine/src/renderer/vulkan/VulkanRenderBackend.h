#pragma once

#include "core/Base.h"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "window/glfw/GLFWWindow.h"

#include "VulkanRenderContext.h"
#include "VulkanUploadContext.h"
#include "VulkanContext.h"
#include "VulkanDescriptorManager.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanResourceAllocator.h"

namespace liquid {

class VulkanRenderBackend {
public:
  /**
   * @brief Create Vulkan render backend
   *
   * @param window GLFW window
   * @param enableValidations Enable validations
   */
  VulkanRenderBackend(GLFWWindow *window, bool enableValidations = false);

  VulkanRenderBackend(const VulkanRenderBackend &rhs) = delete;
  VulkanRenderBackend(VulkanRenderBackend &&rhs) = delete;
  VulkanRenderBackend &operator=(const VulkanRenderBackend &rhs) = delete;
  VulkanRenderBackend &operator=(VulkanRenderBackend &&rhs) = delete;

  /**
   * @brief Destroy Vulkan render backend
   */
  ~VulkanRenderBackend();

  /**
   * @brief Get resource allocator
   *
   * @return Resource allocator
   */
  inline VulkanResourceAllocator *getResourceAllocator() {
    return resourceAllocator;
  }

  /**
   * @brief Get Vulkan instance
   *
   * @return Vulkan instance
   */
  inline VulkanContext &getVulkanInstance() { return vulkanInstance; }

  /**
   * @brief Get window
   *
   * @return Window
   */
  inline GLFWWindow *getWindow() { return window; }

  /**
   * @brief Get render context
   *
   * @return Render context
   */
  inline VulkanRenderContext &getRenderContext() { return renderContext; }

  /**
   * @brief Get swapchain
   *
   * @return Swapchain
   */
  inline VulkanSwapchain &getSwapchain() { return swapchain; }

  /**
   * @brief Get swapchain pass
   *
   * @return Swapchain pass
   */
  inline VkRenderPass getSwapchainPass() { return swapchainPass; }

  /**
   * @brief Get swapchain framebuffers
   *
   * @return Swapchain framebuffers
   */
  inline const std::vector<VkFramebuffer> &getSwapchainFramebuffers() const {
    return swapchainFramebuffers;
  }

  /**
   * @brief Check if framebuffer is resized
   *
   * @return Framebuffer is resized
   */
  inline bool isFramebufferResized() const { return framebufferResized; }

  /**
   * @brief Wait for idle device
   */
  void waitForIdle();

public:
  /**
   * @brief Create allocator
   */
  void createAllocator();

  /**
   * @brief Create swapchain
   */
  void createSwapchain();

  /**
   * @brief Recreate swapchain
   *
   * Recreates swapchain, framebuffers and render pass
   */
  void recreateSwapchain();

  /**
   * @brief Create swapchain pass
   */
  void createSwapchainPass();

  /**
   * @brief Create swapchain framebuffers
   */
  void createSwapchainFramebuffers();

  /**
   * @brief Destroys swapchain pass
   */
  void destroySwapchainPass();

  /**
   * @brief Destroys swapchain framebuffers
   */
  void destroySwapchainFramebuffers();

private:
  uint32_t resizeHandler = 0;
  bool framebufferResized = false;

  GLFWWindow *window = nullptr;

  VkRenderPass swapchainPass = nullptr;
  std::vector<VkFramebuffer> swapchainFramebuffers;

  VmaAllocator allocator = nullptr;
  VulkanResourceAllocator *resourceAllocator = nullptr;
  VulkanContext vulkanInstance;
  VulkanUploadContext uploadContext;
  VulkanSwapchain swapchain;
  VulkanRenderContext renderContext;
};

} // namespace liquid

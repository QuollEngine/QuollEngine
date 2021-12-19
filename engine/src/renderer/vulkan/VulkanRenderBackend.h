#pragma once

#include "core/Base.h"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "window/glfw/GLFWWindow.h"

#include "renderer/render-graph/RenderGraph.h"

#include "VulkanRenderContext.h"
#include "VulkanUploadContext.h"
#include "VulkanContext.h"
#include "VulkanDescriptorManager.h"
#include "VulkanResourceAllocator.h"
#include "VulkanGraphEvaluator.h"

namespace liquid {

class VulkanRenderBackend {
public:
  /**
   * @brief Create Vulkan render backend
   *
   * @param window GLFW window
   * @param enableValidations Enable validations
   */
  VulkanRenderBackend(GLFWWindow *window, bool enableValidations);

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
   * @brief Check if framebuffer is resized
   *
   * @return Framebuffer is resized
   */
  inline bool isFramebufferResized() const { return framebufferResized; }

  /**
   * @brief Get stats manager
   *
   * @return Stats manager
   */
  inline StatsManager &getStatsManager() { return statsManager; }

  /**
   * @brief Execute render graph
   *
   * @param graph Render graph
   */
  void execute(RenderGraph &graph);

  /**
   * @brief Wait for idle device
   */
  void waitForIdle();

public:
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

private:
  uint32_t resizeHandler = 0;
  bool framebufferResized = false;

  GLFWWindow *window = nullptr;

  VmaAllocator allocator = nullptr;
  VulkanResourceAllocator *resourceAllocator = nullptr;
  VulkanContext vulkanInstance;
  VulkanUploadContext uploadContext;
  VulkanSwapchain swapchain;
  VulkanRenderContext renderContext;
  StatsManager statsManager;
  std::unique_ptr<VulkanGraphEvaluator> graphEvaluator;
};

} // namespace liquid

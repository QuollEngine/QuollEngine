#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "liquid/window/glfw/GLFWWindow.h"

#include "liquid/renderer/render-graph/RenderGraph.h"

#include "VulkanRenderContext.h"
#include "VulkanUploadContext.h"
#include "VulkanGraphEvaluator.h"
#include "VulkanDescriptorManager.h"

#include "liquid/rhi/ResourceRegistry.h"
#include "liquid/rhi/vulkan/VulkanRenderDevice.h"

namespace liquid {

/**
 * @deprecated This class will be replaced with RHI
 */
class VulkanAbstraction {
public:
  /**
   * @brief Create Vulkan render backend
   *
   * @param window GLFW window
   * @param device Vulkan render device
   */
  VulkanAbstraction(GLFWWindow *window,
                    experimental::VulkanRenderDevice *device);

  VulkanAbstraction(const VulkanAbstraction &rhs) = delete;
  VulkanAbstraction(VulkanAbstraction &&rhs) = delete;
  VulkanAbstraction &operator=(const VulkanAbstraction &rhs) = delete;
  VulkanAbstraction &operator=(VulkanAbstraction &&rhs) = delete;

  /**
   * @brief Destroy Vulkan render backend
   */
  ~VulkanAbstraction();

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

  /**
   * @brief Get Vulkan device
   *
   * @return Vulkan device
   */
  inline experimental::VulkanRenderDevice *getDevice() { return device; }

  /**
   * @brief Get registry
   *
   * @return Registry
   */
  inline experimental::ResourceRegistry &getRegistry() { return registry; }

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
  bool swapchainRecreated = false;

  experimental::VulkanRenderDevice *device = nullptr;

  GLFWWindow *window = nullptr;

  experimental::ResourceRegistry registry;

  VmaAllocator allocator = nullptr;
  VulkanDescriptorManager descriptorManager;
  VulkanUploadContext uploadContext;
  VulkanSwapchain swapchain;
  VulkanRenderContext renderContext;
  StatsManager statsManager;
  std::unique_ptr<VulkanGraphEvaluator> graphEvaluator;
};

} // namespace liquid

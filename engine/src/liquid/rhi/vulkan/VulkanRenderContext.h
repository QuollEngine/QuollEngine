#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "liquid/window/Window.h"
#include "liquid/profiler/StatsManager.h"
#include "VulkanDescriptorManager.h"
#include "VulkanCommandPool.h"
#include "VulkanSwapchain.h"
#include "VulkanDeviceObject.h"
#include "VulkanQueue.h"

#include "liquid/rhi/RenderCommandList.h"

namespace liquid::experimental {

class VulkanRenderContext {
public:
  static constexpr uint32_t NUM_FRAMES = 2;

public:
  /**
   * @brief Create render context
   *
   * Creates semaphores, fences, and command buffers
   * for rendering
   *
   * @param device Vulkan device
   * @param pool Command pool
   * @param graphicsQueue Graphics queue
   * @param presentQueue Present queue
   */
  VulkanRenderContext(VulkanDeviceObject &device, VulkanCommandPool &pool,
                      VulkanQueue &graphicsQueue, VulkanQueue &presentQueue);

  /**
   * @brief Destroy render context
   *
   * Destroys render semaphores, fences, and command buffers
   */
  ~VulkanRenderContext();

  VulkanRenderContext(const VulkanRenderContext &) = delete;
  VulkanRenderContext(VulkanRenderContext &&) = delete;
  VulkanRenderContext &operator=(const VulkanRenderContext &) = delete;
  VulkanRenderContext &operator=(VulkanRenderContext &&) = delete;

  /**
   * @brief Present to screen
   *
   * @param swapchain Vulkan swapchain
   * @param imageIdx Swapchain image index
   * @return Present queue submit result
   */
  VkResult present(const VulkanSwapchain &swapchain, uint32_t imageIdx);

  /**
   * @brief Get image available semaphoer
   *
   * @return Image available semaphore
   */
  inline VkSemaphore getImageAvailableSemaphore() {
    return imageAvailableSemaphores.at(currentFrame);
  }

  /**
   * @brief Begin Rendering
   *
   * Waits for fences and semaphores; and begins command buffers
   *
   * @return Command buffer for current frame
   */
  RenderCommandList &beginRendering();

  /**
   * @brief End rendering
   *
   * Ends command buffer and submits it to the graphics queue
   */
  void endRendering();

private:
  /**
   * @brief Create render semaphores
   */
  void createSemaphores();

  /**
   * @brief Create render fences
   */
  void createFences();

private:
  uint32_t currentFrame = 0;

  std::array<VkSemaphore, NUM_FRAMES> imageAvailableSemaphores{};
  std::array<VkSemaphore, NUM_FRAMES> renderFinishedSemaphores{};
  std::array<VkFence, NUM_FRAMES> renderFences{};
  std::vector<RenderCommandList> renderCommandLists;

  VulkanQueue &mGraphicsQueue;
  VulkanQueue &mPresentQueue;
  VulkanDeviceObject &mDevice;
};

} // namespace liquid::experimental

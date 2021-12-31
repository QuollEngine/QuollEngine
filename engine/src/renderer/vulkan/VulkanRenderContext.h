#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "window/glfw/GLFWWindow.h"
#include "profiler/StatsManager.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderContext.h"
#include "VulkanCommandExecutor.h"
#include "VulkanDescriptorManager.h"

namespace liquid {

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
   * @param context Vulkan context
   * @param descriptorManager Descriptor manager
   * @param statsManager Stats manager
   */
  VulkanRenderContext(const VulkanContext &context,
                      VulkanDescriptorManager &descriptorManager,
                      StatsManager &statsManager);

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
   * @brief Render commands in command list
   *
   * @param commandList Command list
   */
  void render(RenderCommandList &commandList);

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

private:
  /**
   * @brief Begin Rendering
   *
   * Waits for fences and semaphores; and begins command buffers
   *
   * @return Command buffer for current frame
   */
  VulkanCommandExecutor *beginRendering();

  /**
   * @brief End rendering
   *
   * Ends command buffer and submits it to the graphics queue
   */
  void endRendering();

  /**
   * @brief Create render semaphores
   */
  void createSemaphores();

  /**
   * @brief Create render fences
   */
  void createFences();

  /**
   * @brief Create command pool and buffer
   *
   * @param graphicsQueueFamily Graphics queue family index
   * @param statsManager Stats manager
   */
  void createCommandBuffers(uint32_t graphicsQueueFamily,
                            StatsManager &statsManager);

private:
  uint32_t currentFrame = 0;

  std::array<VulkanCommandExecutor *, NUM_FRAMES> commandExecutors{};
  std::array<VkSemaphore, NUM_FRAMES> imageAvailableSemaphores{};
  std::array<VkSemaphore, NUM_FRAMES> renderFinishedSemaphores{};
  std::array<VkFence, NUM_FRAMES> renderFences{};

  VkCommandPool commandPool = VK_NULL_HANDLE;

  VulkanDescriptorManager &descriptorManager;

  VkQueue graphicsQueue = nullptr;
  VkQueue presentQueue = nullptr;
  VkDevice device = nullptr;
};

} // namespace liquid

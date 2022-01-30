#pragma once

#include "liquid/renderer/RenderCommandList.h"
#include "VulkanDescriptorManager.h"
#include "liquid/profiler/StatsManager.h"

namespace liquid {

class VulkanCommandExecutor {
public:
  /**
   * @brief Create command executor
   *
   * @param commandBuffer Command buffer
   * @param descriptorManager Descriptor manager
   * @param statsManager Stats manager
   */
  VulkanCommandExecutor(VkCommandBuffer commandBuffer,
                        VulkanDescriptorManager &descriptorManager,
                        StatsManager &statsManager);

  /**
   * @brief Execute commands
   *
   * @param commandList Command list
   */
  void execute(const RenderCommandList &commandList);

  /**
   * @brief Get command buffer
   *
   * @return Command buffer
   */
  inline VkCommandBuffer getCommandBuffer() const { return commandBuffer; }

private:
  /**
   * @brief Execute begin render pass command
   *
   * @param command Render pass command
   */
  void
  executeBeginRenderPass(const RenderCommandBeginRenderPass *const command);

  /**
   * @brief Execute end render pass command
   *
   * @param command End render pass command
   */
  void executeEndRenderPass(const RenderCommandEndRenderPass *const command);

  /**
   * @brief Execute bind pipeline command
   *
   * @param command Bind pipeline command
   */
  void executeBindPipeline(const RenderCommandBindPipeline *const command);

  /**
   * @brief Execute bind descriptor command
   *
   * @param command Bind descriptor command
   */
  void executeBindDescriptor(const RenderCommandBindDescriptor *const command);

  /**
   * @brief Execute push constants command
   *
   * @param command Push constants command
   */
  void executePushConstants(const RenderCommandPushConstants *const command);

  /**
   * @brief Execute bind vertex buffer
   *
   * @param command Bind vertex buffer command
   */
  void
  executeBindVertexBuffer(const RenderCommandBindVertexBuffer *const command);

  /**
   * @brief Execute bind index buffer command
   *
   * @param command Bind index buffer command
   */
  void
  executeBindIndexBuffer(const RenderCommandBindIndexBuffer *const command);

  /**
   * @brief Execute set viewport command
   *
   * @param command Set viewport command
   */
  void executeSetViewport(const RenderCommandSetViewport *const command);

  /**
   * @brief Execute set scissor command
   *
   * @param command Set scissor command
   */
  void executeSetScissor(const RenderCommandSetScissor *const command);

  /**
   * @brief Execute draw command
   *
   * @param command Drawcommand
   */
  void executeDraw(const RenderCommandDraw *const command);

  /**
   * @brief Execute draw indexed command
   *
   * @param command Draw indexed command
   */
  void executeDrawIndexed(const RenderCommandDrawIndexed *const command);

private:
  VkCommandBuffer commandBuffer;
  VulkanDescriptorManager &descriptorManager;
  StatsManager &statsManager;
};

} // namespace liquid

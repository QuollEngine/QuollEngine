#include "core/Base.h"
#include "VulkanCommandExecutor.h"
#include "VulkanHardwareBuffer.h"
#include "VulkanPipeline.h"
#include "VulkanError.h"

namespace liquid {

VulkanCommandExecutor::VulkanCommandExecutor(
    VkCommandBuffer commandBuffer_, VulkanDescriptorManager &descriptorManager_,
    StatsManager &statsManager_)
    : commandBuffer(commandBuffer_), descriptorManager(descriptorManager_),
      statsManager(statsManager_) {}

void VulkanCommandExecutor::execute(const RenderCommandList &commandList) {
  vkResetCommandBuffer(commandBuffer, 0);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  checkForVulkanError(vkBeginCommandBuffer(commandBuffer, &beginInfo),
                      "Failed to begin recording command buffer");

  for (const auto &command : commandList.getRecordedCommands()) {
    switch (command->type) {
    case RenderCommandType::BeginRenderPass:
      executeBeginRenderPass(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandBeginRenderPass *>(command.get()));
      break;
    case RenderCommandType::EndRenderPass:
      executeEndRenderPass(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandEndRenderPass *>(command.get()));
      break;
    case RenderCommandType::BindPipeline:
      executeBindPipeline(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandBindPipeline *>(command.get()));
      break;
    case RenderCommandType::BindDescriptor:
      executeBindDescriptor(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandBindDescriptor *>(command.get()));
      break;
    case RenderCommandType::PushConstants:
      executePushConstants(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandPushConstants *>(command.get()));
      break;
    case RenderCommandType::BindVertexBuffer:
      executeBindVertexBuffer(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandBindVertexBuffer *>(command.get()));
      break;
    case RenderCommandType::BindIndexBuffer:
      executeBindIndexBuffer(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandBindIndexBuffer *>(command.get()));
      break;
    case RenderCommandType::SetViewport:
      executeSetViewport(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandSetViewport *const>(command.get()));
      break;
    case RenderCommandType::SetScissor:
      executeSetScissor(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandSetScissor *const>(command.get()));
      break;
    case RenderCommandType::Draw:
      executeDraw(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandDraw *const>(command.get()));
      break;
    case RenderCommandType::DrawIndexed:
      executeDrawIndexed(
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
          static_cast<const RenderCommandDrawIndexed *const>(command.get()));
      break;
    default:
      break;
    }
  }

  checkForVulkanError(vkEndCommandBuffer(commandBuffer),
                      "Failed to finish recording command buffer");
}

void VulkanCommandExecutor::executeBeginRenderPass(
    const RenderCommandBeginRenderPass *const command) {
  std::vector<VkClearValue> clearValues(command->clearValues.size());

  VkRenderPassBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  beginInfo.pNext = nullptr;
  beginInfo.framebuffer = command->framebuffer;
  beginInfo.renderPass = command->renderPass;
  beginInfo.renderArea.offset = {command->renderAreaOffset.x,
                                 command->renderAreaOffset.y};
  beginInfo.renderArea.extent = {command->renderAreaSize.x,
                                 command->renderAreaSize.y};
  beginInfo.clearValueCount =
      static_cast<uint32_t>(command->clearValues.size());
  beginInfo.pClearValues = command->clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandExecutor::executeEndRenderPass(
    const RenderCommandEndRenderPass *const command) {
  vkCmdEndRenderPass(commandBuffer);
}

void VulkanCommandExecutor::executeBindPipeline(
    const RenderCommandBindPipeline *const command) {
  const auto &pipeline =
      std::dynamic_pointer_cast<VulkanPipeline>(command->pipeline);
  vkCmdBindPipeline(commandBuffer, pipeline->getBindPoint(),
                    pipeline->getPipeline());
}

void VulkanCommandExecutor::executeBindDescriptor(
    const RenderCommandBindDescriptor *const command) {
  const auto &pipeline =
      std::dynamic_pointer_cast<VulkanPipeline>(command->pipeline);

  VkDescriptorSet descriptorSet = descriptorManager.getOrCreateDescriptor(
      command->descriptor, pipeline->getDescriptorLayout(command->firstSet));

  vkCmdBindDescriptorSets(commandBuffer, pipeline->getBindPoint(),
                          pipeline->getPipelineLayout(), command->firstSet, 1,
                          &descriptorSet, 0, {});
}

void VulkanCommandExecutor::executePushConstants(
    const RenderCommandPushConstants *const command) {
  const auto &pipeline =
      std::dynamic_pointer_cast<VulkanPipeline>(command->pipeline);
  vkCmdPushConstants(commandBuffer, pipeline->getPipelineLayout(),
                     command->stageFlags, command->offset, command->size,
                     command->data.get());
}

void VulkanCommandExecutor::executeBindVertexBuffer(
    const RenderCommandBindVertexBuffer *const command) {
  const auto &buffer =
      std::dynamic_pointer_cast<VulkanHardwareBuffer>(command->buffer)
          ->getBuffer();
  std::array<VkDeviceSize, 1> offsets{0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, offsets.data());
}

void VulkanCommandExecutor::executeBindIndexBuffer(
    const RenderCommandBindIndexBuffer *const command) {
  const auto &buffer =
      std::dynamic_pointer_cast<VulkanHardwareBuffer>(command->buffer)
          ->getBuffer();
  vkCmdBindIndexBuffer(commandBuffer, buffer, 0, command->indexType);
}

void VulkanCommandExecutor::executeSetViewport(
    const RenderCommandSetViewport *const command) {
  VkViewport viewport{};
  viewport.x = command->offset.x;
  viewport.y = command->offset.y;
  viewport.width = command->size.x;
  viewport.height = command->size.y;
  viewport.minDepth = command->depthRange.x;
  viewport.maxDepth = command->depthRange.y;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void VulkanCommandExecutor::executeSetScissor(
    const RenderCommandSetScissor *const command) {
  VkRect2D scissor{};
  scissor.offset = {command->offset.x, command->offset.y};
  scissor.extent = {command->size.x, command->size.y};
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VulkanCommandExecutor::executeDraw(
    const RenderCommandDraw *const command) {
  vkCmdDraw(commandBuffer, static_cast<uint32_t>(command->vertexCount), 1,
            command->firstVertex, 0);
  statsManager.addDrawCall(command->vertexCount / 3);
}

void VulkanCommandExecutor::executeDrawIndexed(
    const RenderCommandDrawIndexed *const command) {
  vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(command->indexCount), 1,
                   command->firstIndex, command->vertexOffset, 0);
  statsManager.addDrawCall(command->indexCount / 3);
}

} // namespace liquid

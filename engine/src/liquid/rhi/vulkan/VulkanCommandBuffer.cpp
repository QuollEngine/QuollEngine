#include "liquid/core/Base.h"

#include "liquid/renderer/vulkan/VulkanPipeline.h"
#include "VulkanCommandBuffer.h"

namespace liquid::experimental {

VulkanCommandBuffer::VulkanCommandBuffer(
    VkCommandBuffer commandBuffer, const VulkanResourceRegistry &registry,
    VulkanDescriptorManager &descriptorManager)
    : mCommandBuffer(commandBuffer), mRegistry(registry),
      mDescriptorManager(descriptorManager) {}

void VulkanCommandBuffer::beginRenderPass(
    VkRenderPass renderPass, VkFramebuffer framebuffer,
    const glm::ivec2 &renderAreaOffset, const glm::uvec2 &renderAreaSize,
    const std::vector<VkClearValue> &clearValues) {
  VkRenderPassBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  beginInfo.pNext = nullptr;
  beginInfo.framebuffer = framebuffer;
  beginInfo.renderPass = renderPass;
  beginInfo.renderArea.offset = {renderAreaOffset.x, renderAreaOffset.y};
  beginInfo.renderArea.extent = {renderAreaSize.x, renderAreaSize.y};
  beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  beginInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(mCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::endRenderPass() {
  vkCmdEndRenderPass(mCommandBuffer);
}

void VulkanCommandBuffer::bindPipeline(const SharedPtr<Pipeline> &pipeline) {
  const auto &vulkanPipeline =
      std::dynamic_pointer_cast<VulkanPipeline>(pipeline);

  vkCmdBindPipeline(mCommandBuffer, vulkanPipeline->getBindPoint(),
                    vulkanPipeline->getPipeline());
}

void VulkanCommandBuffer::bindDescriptor(const SharedPtr<Pipeline> &pipeline,
                                         uint32_t firstSet,
                                         const Descriptor &descriptor) {
  const auto &vulkanPipeline =
      std::dynamic_pointer_cast<VulkanPipeline>(pipeline);
  VkDescriptorSet descriptorSet = mDescriptorManager.getOrCreateDescriptor(
      descriptor, vulkanPipeline->getDescriptorLayout(firstSet));

  vkCmdBindDescriptorSets(mCommandBuffer, vulkanPipeline->getBindPoint(),
                          vulkanPipeline->getPipelineLayout(), firstSet, 1,
                          &descriptorSet, 0, {});
}

void VulkanCommandBuffer::bindVertexBuffer(BufferHandle buffer) {
  const auto &vulkanBuffer = mRegistry.getBuffer(buffer);
  std::array<VkDeviceSize, 1> offsets{0};
  std::array<VkBuffer, 1> buffers{vulkanBuffer->getBuffer()};

  vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, buffers.data(), offsets.data());
}

void VulkanCommandBuffer::bindIndexBuffer(BufferHandle buffer,
                                          VkIndexType indexType) {
  const auto &vulkanBuffer = mRegistry.getBuffer(buffer);

  vkCmdBindIndexBuffer(mCommandBuffer, vulkanBuffer->getBuffer(), 0, indexType);
}

void VulkanCommandBuffer::pushConstants(const SharedPtr<Pipeline> &pipeline,
                                        VkShaderStageFlags stageFlags,
                                        uint32_t offset, uint32_t size,
                                        void *data) {
  const auto &vulkanPipeline =
      std::dynamic_pointer_cast<VulkanPipeline>(pipeline);

  vkCmdPushConstants(mCommandBuffer, vulkanPipeline->getPipelineLayout(),
                     stageFlags, offset, size, data);
}

void VulkanCommandBuffer::draw(uint32_t vertexCount, uint32_t firstVertex) {
  vkCmdDraw(mCommandBuffer, vertexCount, 1, firstVertex, 0);
}

void VulkanCommandBuffer::drawIndexed(uint32_t indexCount, uint32_t firstIndex,
                                      int32_t vertexOffset) {
  vkCmdDrawIndexed(mCommandBuffer, indexCount, 1, firstIndex, vertexOffset, 0);
}

void VulkanCommandBuffer::setViewport(const glm::vec2 &offset,
                                      const glm::vec2 &size,
                                      const glm::vec2 &depthRange) {
  VkViewport viewport{offset.x, offset.y,     size.x,
                      size.y,   depthRange.x, depthRange.y};

  vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
}

void VulkanCommandBuffer::setScissor(const glm::ivec2 &offset,
                                     const glm::uvec2 &size) {
  VkRect2D scissor{VkOffset2D{offset.x, offset.y}, VkExtent2D{size.x, size.y}};

  vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);
}

} // namespace liquid::experimental

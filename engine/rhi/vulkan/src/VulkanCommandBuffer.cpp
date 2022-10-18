#include "liquid/core/Base.h"

#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"

namespace liquid::rhi {

VulkanCommandBuffer::VulkanCommandBuffer(
    VkCommandBuffer commandBuffer, const VulkanResourceRegistry &registry,
    VulkanDescriptorManager &descriptorManager, DeviceStats &stats)
    : mCommandBuffer(commandBuffer), mRegistry(registry),
      mDescriptorManager(descriptorManager), mStats(stats) {}

void VulkanCommandBuffer::beginRenderPass(rhi::RenderPassHandle renderPass,
                                          FramebufferHandle framebuffer,
                                          const glm::ivec2 &renderAreaOffset,
                                          const glm::uvec2 &renderAreaSize) {

  const auto &vulkanRenderPass = mRegistry.getRenderPasses().at(renderPass);

  VkRenderPassBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  beginInfo.pNext = nullptr;
  beginInfo.framebuffer =
      mRegistry.getFramebuffers().at(framebuffer)->getFramebuffer();
  beginInfo.renderPass = vulkanRenderPass->getRenderPass();
  beginInfo.renderArea.offset = {renderAreaOffset.x, renderAreaOffset.y};
  beginInfo.renderArea.extent = {renderAreaSize.x, renderAreaSize.y};
  beginInfo.clearValueCount =
      static_cast<uint32_t>(vulkanRenderPass->getClearValues().size());
  beginInfo.pClearValues = vulkanRenderPass->getClearValues().data();

  vkCmdBeginRenderPass(mCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::endRenderPass() {
  vkCmdEndRenderPass(mCommandBuffer);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::bindPipeline(PipelineHandle pipeline) {
  const auto &vulkanPipeline = mRegistry.getPipelines().at(pipeline);

  vkCmdBindPipeline(mCommandBuffer, vulkanPipeline->getBindPoint(),
                    vulkanPipeline->getPipeline());
  mStats.addCommandCall();
}

void VulkanCommandBuffer::bindDescriptor(PipelineHandle pipeline,
                                         uint32_t firstSet,
                                         const Descriptor &descriptor) {
  const auto &vulkanPipeline = mRegistry.getPipelines().at(pipeline);
  VkDescriptorSet descriptorSet = mDescriptorManager.getOrCreateDescriptor(
      descriptor, vulkanPipeline->getDescriptorLayout(firstSet));

  vkCmdBindDescriptorSets(mCommandBuffer, vulkanPipeline->getBindPoint(),
                          vulkanPipeline->getPipelineLayout(), firstSet, 1,
                          &descriptorSet, 0, {});
  mStats.addCommandCall();
}

void VulkanCommandBuffer::bindVertexBuffer(BufferHandle buffer) {
  const auto &vulkanBuffer = mRegistry.getBuffers().at(buffer);
  std::array<VkDeviceSize, 1> offsets{0};
  std::array<VkBuffer, 1> buffers{vulkanBuffer->getBuffer()};

  vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, buffers.data(), offsets.data());
  mStats.addCommandCall();
}

void VulkanCommandBuffer::bindIndexBuffer(BufferHandle buffer,
                                          VkIndexType indexType) {
  const auto &vulkanBuffer = mRegistry.getBuffers().at(buffer);

  vkCmdBindIndexBuffer(mCommandBuffer, vulkanBuffer->getBuffer(), 0, indexType);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::pushConstants(PipelineHandle pipeline,
                                        VkShaderStageFlags stageFlags,
                                        uint32_t offset, uint32_t size,
                                        void *data) {
  const auto &vulkanPipeline = mRegistry.getPipelines().at(pipeline);

  vkCmdPushConstants(mCommandBuffer, vulkanPipeline->getPipelineLayout(),
                     stageFlags, offset, size, data);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::draw(uint32_t vertexCount, uint32_t firstVertex,
                               uint32_t instanceCount, uint32_t firstInstance) {
  vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, firstVertex,
            firstInstance);
  mStats.addDrawCall(vertexCount / 3);
}

void VulkanCommandBuffer::drawIndexed(uint32_t indexCount, uint32_t firstIndex,
                                      int32_t vertexOffset,
                                      uint32_t instanceCount,
                                      uint32_t firstInstance) {
  vkCmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, firstIndex,
                   vertexOffset, firstInstance);
  mStats.addDrawCall(indexCount / 3);
}

void VulkanCommandBuffer::setViewport(const glm::vec2 &offset,
                                      const glm::vec2 &size,
                                      const glm::vec2 &depthRange) {
  // Using negative height to flip the viewport
  VkViewport viewport{offset.x, size.y - offset.y, size.x,
                      -size.y,  depthRange.x,      depthRange.y};

  vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::setScissor(const glm::ivec2 &offset,
                                     const glm::uvec2 &size) {
  VkRect2D scissor{VkOffset2D{offset.x, offset.y}, VkExtent2D{size.x, size.y}};

  vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::pipelineBarrier(
    VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,
    const std::vector<MemoryBarrier> &memoryBarriers,
    const std::vector<ImageBarrier> &imageBarriers) {

  std::vector<VkMemoryBarrier> vkMemoryBarriers(memoryBarriers.size());
  for (size_t i = 0; i < memoryBarriers.size(); ++i) {
    auto &barrier = memoryBarriers.at(i);
    auto &vkBarrier = vkMemoryBarriers.at(i);
    vkBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    vkBarrier.pNext = nullptr;
    vkBarrier.srcAccessMask = barrier.srcAccess;
    vkBarrier.dstAccessMask = barrier.dstAccess;
  }

  std::vector<VkImageMemoryBarrier> vkImageMemoryBarriers(imageBarriers.size());
  for (size_t i = 0; i < imageBarriers.size(); ++i) {
    auto &barrier = imageBarriers.at(i);
    const auto &texture = mRegistry.getTextures().at(barrier.texture);
    auto &vkBarrier = vkImageMemoryBarriers.at(i);
    vkBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    vkBarrier.pNext = nullptr;
    vkBarrier.srcAccessMask = barrier.srcAccess;
    vkBarrier.dstAccessMask = barrier.dstAccess;
    vkBarrier.oldLayout = barrier.srcLayout;
    vkBarrier.newLayout = barrier.dstLayout;
    vkBarrier.image = texture->getImage();
    vkBarrier.subresourceRange.baseArrayLayer = 0;
    vkBarrier.subresourceRange.baseMipLevel = 0;
    vkBarrier.subresourceRange.layerCount = texture->getDescription().layers;
    vkBarrier.subresourceRange.levelCount = 1;
    vkBarrier.subresourceRange.aspectMask = texture->getImageAspectFlags();
  }

  vkCmdPipelineBarrier(
      mCommandBuffer, srcStage, dstStage, VK_DEPENDENCY_BY_REGION_BIT,
      static_cast<uint32_t>(vkMemoryBarriers.size()), vkMemoryBarriers.data(),
      0, nullptr, static_cast<uint32_t>(vkImageMemoryBarriers.size()),
      vkImageMemoryBarriers.data());
}

} // namespace liquid::rhi

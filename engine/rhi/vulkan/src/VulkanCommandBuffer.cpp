#include "liquid/core/Base.h"

#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanMapping.h"

namespace liquid::rhi {

VulkanCommandBuffer::VulkanCommandBuffer(
    VkCommandBuffer commandBuffer, const VulkanResourceRegistry &registry,
    const VulkanDescriptorPool &descriptorPool, DeviceStats &stats)
    : mCommandBuffer(commandBuffer), mRegistry(registry),
      mDescriptorPool(descriptorPool), mStats(stats) {}

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

void VulkanCommandBuffer::bindDescriptor(
    PipelineHandle pipeline, uint32_t firstSet, const Descriptor &descriptor,
    const std::vector<uint32_t> &dynamicOffsets) {
  const auto &vulkanPipeline = mRegistry.getPipelines().at(pipeline);
  VkDescriptorSet descriptorSet =
      mDescriptorPool.getDescriptorSet(descriptor.getHandle());

  vkCmdBindDescriptorSets(
      mCommandBuffer, vulkanPipeline->getBindPoint(),
      vulkanPipeline->getPipelineLayout(), firstSet, 1, &descriptorSet,
      static_cast<uint32_t>(dynamicOffsets.size()), dynamicOffsets.data());
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
                                          IndexType indexType) {
  const auto &vulkanBuffer = mRegistry.getBuffers().at(buffer);

  vkCmdBindIndexBuffer(mCommandBuffer, vulkanBuffer->getBuffer(), 0,
                       VulkanMapping::getIndexType(indexType));
  mStats.addCommandCall();
}

void VulkanCommandBuffer::pushConstants(PipelineHandle pipeline,
                                        ShaderStage shaderStage,
                                        uint32_t offset, uint32_t size,
                                        void *data) {
  const auto &vulkanPipeline = mRegistry.getPipelines().at(pipeline);

  vkCmdPushConstants(mCommandBuffer, vulkanPipeline->getPipelineLayout(),
                     VulkanMapping::getShaderStageFlags(shaderStage), offset,
                     size, data);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::draw(uint32_t vertexCount, uint32_t firstVertex,
                               uint32_t instanceCount, uint32_t firstInstance) {
  vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, firstVertex,
            firstInstance);
  mStats.addDrawCall((vertexCount / 3) * instanceCount);
}

void VulkanCommandBuffer::drawIndexed(uint32_t indexCount, uint32_t firstIndex,
                                      int32_t vertexOffset,
                                      uint32_t instanceCount,
                                      uint32_t firstInstance) {
  vkCmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, firstIndex,
                   vertexOffset, firstInstance);
  mStats.addDrawCall((indexCount / 3) * instanceCount);
}

void VulkanCommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY,
                                   uint32_t groupCountZ) {
  vkCmdDispatch(mCommandBuffer, groupCountX, groupCountY, groupCountZ);
  mStats.addCommandCall();
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
    PipelineStage srcStage, PipelineStage dstStage,
    const std::vector<MemoryBarrier> &memoryBarriers,
    const std::vector<ImageBarrier> &imageBarriers) {

  std::vector<VkMemoryBarrier> vkMemoryBarriers(memoryBarriers.size());
  for (size_t i = 0; i < memoryBarriers.size(); ++i) {
    auto &barrier = memoryBarriers.at(i);
    auto &vkBarrier = vkMemoryBarriers.at(i);
    vkBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    vkBarrier.pNext = nullptr;
    vkBarrier.srcAccessMask = VulkanMapping::getAccessFlags(barrier.srcAccess);
    vkBarrier.dstAccessMask = VulkanMapping::getAccessFlags(barrier.dstAccess);
  }

  std::vector<VkImageMemoryBarrier> vkImageMemoryBarriers(imageBarriers.size());
  for (size_t i = 0; i < imageBarriers.size(); ++i) {
    auto &barrier = imageBarriers.at(i);
    const auto &texture = mRegistry.getTextures().at(barrier.texture);
    auto &vkBarrier = vkImageMemoryBarriers.at(i);
    vkBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    vkBarrier.pNext = nullptr;
    vkBarrier.srcAccessMask = VulkanMapping::getAccessFlags(barrier.srcAccess);
    vkBarrier.dstAccessMask = VulkanMapping::getAccessFlags(barrier.dstAccess);
    vkBarrier.oldLayout = VulkanMapping::getImageLayout(barrier.srcLayout);
    vkBarrier.newLayout = VulkanMapping::getImageLayout(barrier.dstLayout);
    vkBarrier.image = texture->getImage();
    vkBarrier.subresourceRange.baseArrayLayer = 0;
    vkBarrier.subresourceRange.baseMipLevel = barrier.baseLevel;
    vkBarrier.subresourceRange.layerCount = texture->getDescription().layers;
    vkBarrier.subresourceRange.levelCount = barrier.levelCount;
    vkBarrier.subresourceRange.aspectMask = texture->getImageAspectFlags();
  }

  vkCmdPipelineBarrier(
      mCommandBuffer, VulkanMapping::getPipelineStageFlags(srcStage),
      VulkanMapping::getPipelineStageFlags(dstStage),
      VK_DEPENDENCY_BY_REGION_BIT,
      static_cast<uint32_t>(vkMemoryBarriers.size()), vkMemoryBarriers.data(),
      0, nullptr, static_cast<uint32_t>(vkImageMemoryBarriers.size()),
      vkImageMemoryBarriers.data());
}

void VulkanCommandBuffer::copyTextureToBuffer(
    TextureHandle srcTexture, BufferHandle dstBuffer,
    const std::vector<CopyRegion> &copyRegions) {
  const auto &vulkanTexture = mRegistry.getTextures().at(srcTexture);
  const auto &vulkanBuffer = mRegistry.getBuffers().at(dstBuffer);

  std::vector<VkBufferImageCopy> copies(copyRegions.size());
  for (size_t i = 0; i < copies.size(); ++i) {
    auto &copy = copies.at(i);
    auto &copyRegion = copyRegions.at(i);
    copy.bufferOffset = copyRegion.bufferOffset;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.baseArrayLayer = copyRegion.imageBaseArrayLayer;
    copy.imageSubresource.layerCount = copyRegion.imageLayerCount;
    copy.imageSubresource.mipLevel = copyRegion.imageLevel;
    copy.imageOffset =
        VkOffset3D{copyRegion.imageOffset.x, copyRegion.imageOffset.y,
                   copyRegion.imageOffset.z};
    copy.imageExtent =
        VkExtent3D{copyRegion.imageExtent.x, copyRegion.imageExtent.y,
                   copyRegion.imageExtent.z};
  }

  vkCmdCopyImageToBuffer(mCommandBuffer, vulkanTexture->getImage(),
                         VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                         vulkanBuffer->getBuffer(),
                         static_cast<uint32_t>(copies.size()), copies.data());
}

void VulkanCommandBuffer::copyBufferToTexture(
    BufferHandle srcBuffer, TextureHandle dstTexture,
    const std::vector<CopyRegion> &copyRegions) {
  const auto &vulkanBuffer = mRegistry.getBuffers().at(srcBuffer);
  const auto &vulkanTexture = mRegistry.getTextures().at(dstTexture);

  std::vector<VkBufferImageCopy> copies(copyRegions.size());
  for (size_t i = 0; i < copies.size(); ++i) {
    auto &copy = copies.at(i);
    auto &copyRegion = copyRegions.at(i);
    copy.bufferOffset = copyRegion.bufferOffset;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.baseArrayLayer = copyRegion.imageBaseArrayLayer;
    copy.imageSubresource.layerCount = copyRegion.imageLayerCount;
    copy.imageSubresource.mipLevel = copyRegion.imageLevel;
    copy.imageOffset =
        VkOffset3D{copyRegion.imageOffset.x, copyRegion.imageOffset.y,
                   copyRegion.imageOffset.z};
    copy.imageExtent =
        VkExtent3D{copyRegion.imageExtent.x, copyRegion.imageExtent.y,
                   copyRegion.imageExtent.z};
  }

  vkCmdCopyBufferToImage(mCommandBuffer, vulkanBuffer->getBuffer(),
                         vulkanTexture->getImage(),
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         static_cast<uint32_t>(copies.size()), copies.data());
}

void VulkanCommandBuffer::blitTexture(TextureHandle source,
                                      TextureHandle destination,
                                      const std::vector<BlitRegion> &regions,
                                      Filter filter) {
  VkImage srcImage = mRegistry.getTextures().at(source)->getImage();
  VkImage dstImage = mRegistry.getTextures().at(destination)->getImage();

  static constexpr size_t OffsetSize = 2;
  std::vector<VkImageBlit> vkRegions(regions.size());
  for (size_t i = 0; i < regions.size(); ++i) {
    auto &region = regions.at(i);
    auto &vkRegion = vkRegions.at(i);

    vkRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vkRegion.srcSubresource.mipLevel = region.srcMipLevel;
    vkRegion.srcSubresource.baseArrayLayer = region.srcBaseArrayLayer;
    vkRegion.srcSubresource.layerCount = region.srcLayerCount;

    for (size_t i = 0; i < OffsetSize; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      vkRegion.srcOffsets[i] = {region.srcOffsets.at(i).x,
                                region.srcOffsets.at(i).y,
                                region.srcOffsets.at(i).z};
    }

    vkRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vkRegion.dstSubresource.mipLevel = region.dstMipLevel;
    vkRegion.dstSubresource.baseArrayLayer = region.dstBaseArrayLayer;
    vkRegion.dstSubresource.layerCount = region.dstLayerCount;
    for (size_t i = 0; i < OffsetSize; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      vkRegion.dstOffsets[i] = {region.dstOffsets.at(i).x,
                                region.dstOffsets.at(i).y,
                                region.dstOffsets.at(i).z};
    }
  }

  vkCmdBlitImage(mCommandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                 dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                 static_cast<uint32_t>(regions.size()), vkRegions.data(),
                 VulkanMapping::getFilter(filter));
}

} // namespace liquid::rhi

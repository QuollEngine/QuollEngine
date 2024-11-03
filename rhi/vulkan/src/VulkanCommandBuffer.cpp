#include "quoll/core/Base.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanMapping.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanTexture.h"

namespace quoll::rhi {

VulkanCommandBuffer::VulkanCommandBuffer(
    VkCommandBuffer commandBuffer, const VulkanResourceRegistry &registry,
    const VulkanDescriptorPool &descriptorPool,
    const VulkanTimestampManager &timestampManager, DeviceStats &stats)
    : mCommandBuffer(commandBuffer), mRegistry(registry),
      mDescriptorPool(descriptorPool), mTimestampManager(timestampManager),
      mStats(stats) {}

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
      static_cast<u32>(vulkanRenderPass->getClearValues().size());
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

void VulkanCommandBuffer::bindDescriptor(PipelineHandle pipeline, u32 firstSet,
                                         const Descriptor &descriptor,
                                         std::span<u32> dynamicOffsets) {
  const auto &vulkanPipeline = mRegistry.getPipelines().at(pipeline);
  VkDescriptorSet descriptorSet =
      mDescriptorPool.getDescriptorSet(descriptor.getHandle());

  QuollAssert(
      vulkanPipeline->getDescriptorLayout(firstSet) ==
          mDescriptorPool.getLayoutFromDescriptor(descriptorSet),
      "Layout of provided descriptor does not match the layout for set #" +
          std::to_string(firstSet) + " that is defined in pipeline \"" +
          vulkanPipeline->getDebugName() + "\"");

  vkCmdBindDescriptorSets(
      mCommandBuffer, vulkanPipeline->getBindPoint(),
      vulkanPipeline->getPipelineLayout(), firstSet, 1, &descriptorSet,
      static_cast<u32>(dynamicOffsets.size()), dynamicOffsets.data());
  mStats.addCommandCall();
}

void VulkanCommandBuffer::bindVertexBuffers(
    const std::span<const BufferHandle> buffers,
    const std::span<const u64> offsets) {
  QuollAssert(buffers.size() == offsets.size(),
              "Buffers and offsets must match");

  std::vector<VkBuffer> vkBuffers;
  for (auto handle : buffers) {
    vkBuffers.push_back(mRegistry.getBuffers().at(handle)->getBuffer());
  }

  vkCmdBindVertexBuffers(mCommandBuffer, 0, static_cast<u32>(vkBuffers.size()),
                         vkBuffers.data(), offsets.data());
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
                                        ShaderStage shaderStage, u32 offset,
                                        u32 size, void *data) {
  const auto &vulkanPipeline = mRegistry.getPipelines().at(pipeline);

  vkCmdPushConstants(mCommandBuffer, vulkanPipeline->getPipelineLayout(),
                     VulkanMapping::getShaderStageFlags(shaderStage), offset,
                     size, data);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::draw(u32 vertexCount, u32 firstVertex,
                               u32 instanceCount, u32 firstInstance) {
  vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, firstVertex,
            firstInstance);
  mStats.addDrawCall((vertexCount / 3) * instanceCount);
}

void VulkanCommandBuffer::drawIndexed(u32 indexCount, u32 firstIndex,
                                      i32 vertexOffset, u32 instanceCount,
                                      u32 firstInstance) {
  vkCmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, firstIndex,
                   vertexOffset, firstInstance);
  mStats.addDrawCall((indexCount / 3) * instanceCount);
}

void VulkanCommandBuffer::dispatch(u32 groupCountX, u32 groupCountY,
                                   u32 groupCountZ) {
  vkCmdDispatch(mCommandBuffer, groupCountX, groupCountY, groupCountZ);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::setViewport(const glm::vec2 &offset,
                                      const glm::vec2 &size,
                                      const glm::vec2 &depthRange) {
  // Using negative height to flip the viewport
  const VkViewport viewport{offset.x, size.y - offset.y, size.x,
                            -size.y,  depthRange.x,      depthRange.y};

  vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::setScissor(const glm::ivec2 &offset,
                                     const glm::uvec2 &size) {
  const VkRect2D scissor{VkOffset2D{offset.x, offset.y},
                         VkExtent2D{size.x, size.y}};

  vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);
  mStats.addCommandCall();
}

void VulkanCommandBuffer::pipelineBarrier(
    std::span<MemoryBarrier> memoryBarriers,
    std::span<ImageBarrier> imageBarriers,
    std::span<BufferBarrier> bufferBarriers) {

  std::vector<VkMemoryBarrier2> vkMemoryBarriers(memoryBarriers.size());
  for (usize i = 0; i < memoryBarriers.size(); ++i) {
    auto &barrier = memoryBarriers[i];
    auto &vkBarrier = vkMemoryBarriers.at(i);
    vkBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
    vkBarrier.pNext = nullptr;
    vkBarrier.srcAccessMask = VulkanMapping::getAccessFlags(barrier.srcAccess);
    vkBarrier.dstAccessMask = VulkanMapping::getAccessFlags(barrier.dstAccess);
    vkBarrier.srcStageMask =
        VulkanMapping::getPipelineStageFlags(barrier.srcStage);
    vkBarrier.dstStageMask =
        VulkanMapping::getPipelineStageFlags(barrier.dstStage);
  }

  std::vector<VkImageMemoryBarrier2> vkImageMemoryBarriers(
      imageBarriers.size());
  for (usize i = 0; i < imageBarriers.size(); ++i) {
    auto &barrier = imageBarriers[i];
    const auto &texture = mRegistry.getTextures().at(barrier.texture);
    auto &vkBarrier = vkImageMemoryBarriers.at(i);
    vkBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    vkBarrier.pNext = nullptr;
    vkBarrier.srcAccessMask = VulkanMapping::getAccessFlags(barrier.srcAccess);
    vkBarrier.dstAccessMask = VulkanMapping::getAccessFlags(barrier.dstAccess);
    vkBarrier.oldLayout = VulkanMapping::getImageLayout(barrier.srcLayout);
    vkBarrier.newLayout = VulkanMapping::getImageLayout(barrier.dstLayout);
    vkBarrier.image = texture->getImage();
    vkBarrier.subresourceRange.baseArrayLayer = 0;
    vkBarrier.subresourceRange.baseMipLevel = barrier.baseLevel;
    vkBarrier.subresourceRange.layerCount =
        texture->getDescription().layerCount;
    vkBarrier.subresourceRange.levelCount = barrier.levelCount;
    vkBarrier.subresourceRange.aspectMask = texture->getImageAspectFlags();
    vkBarrier.srcStageMask =
        VulkanMapping::getPipelineStageFlags(barrier.srcStage);
    vkBarrier.dstStageMask =
        VulkanMapping::getPipelineStageFlags(barrier.dstStage);
  }

  std::vector<VkBufferMemoryBarrier2> vkBufferMemoryBarriers(
      bufferBarriers.size());
  for (usize i = 0; i < bufferBarriers.size(); ++i) {
    auto &barrier = bufferBarriers[i];
    auto &vkBarrier = vkBufferMemoryBarriers.at(i);
    vkBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    vkBarrier.pNext = nullptr;
    vkBarrier.srcAccessMask = VulkanMapping::getAccessFlags(barrier.srcAccess);
    vkBarrier.dstAccessMask = VulkanMapping::getAccessFlags(barrier.dstAccess);
    vkBarrier.buffer = mRegistry.getBuffers().at(barrier.buffer)->getBuffer();
    vkBarrier.offset = barrier.offset;
    vkBarrier.size = barrier.size > 0 ? barrier.size : VK_WHOLE_SIZE;
    vkBarrier.srcStageMask =
        VulkanMapping::getPipelineStageFlags(barrier.srcStage);
    vkBarrier.dstStageMask =
        VulkanMapping::getPipelineStageFlags(barrier.dstStage);
  }

  VkDependencyInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  info.pNext = nullptr;
  info.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
  info.bufferMemoryBarrierCount =
      static_cast<u32>(vkBufferMemoryBarriers.size());
  info.pBufferMemoryBarriers = vkBufferMemoryBarriers.data();
  info.imageMemoryBarrierCount = static_cast<u32>(vkImageMemoryBarriers.size());
  info.pImageMemoryBarriers = vkImageMemoryBarriers.data();
  info.memoryBarrierCount = static_cast<u32>(vkMemoryBarriers.size());
  info.pMemoryBarriers = vkMemoryBarriers.data();

  vkCmdPipelineBarrier2KHR(mCommandBuffer, &info);
}

void VulkanCommandBuffer::copyTextureToBuffer(
    TextureHandle srcTexture, BufferHandle dstBuffer,
    std::span<CopyRegion> copyRegions) {
  const auto &vulkanTexture = mRegistry.getTextures().at(srcTexture);
  const auto &vulkanBuffer = mRegistry.getBuffers().at(dstBuffer);

  std::vector<VkBufferImageCopy> copies(copyRegions.size());
  for (usize i = 0; i < copies.size(); ++i) {
    auto &copy = copies.at(i);
    auto &copyRegion = copyRegions[i];
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
                         static_cast<u32>(copies.size()), copies.data());
}

void VulkanCommandBuffer::copyBufferToTexture(
    BufferHandle srcBuffer, TextureHandle dstTexture,
    std::span<CopyRegion> copyRegions) {
  const auto &vulkanBuffer = mRegistry.getBuffers().at(srcBuffer);
  const auto &vulkanTexture = mRegistry.getTextures().at(dstTexture);

  std::vector<VkBufferImageCopy> copies(copyRegions.size());
  for (usize i = 0; i < copies.size(); ++i) {
    auto &copy = copies.at(i);
    auto &copyRegion = copyRegions[i];
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
                         static_cast<u32>(copies.size()), copies.data());
}

void VulkanCommandBuffer::blitTexture(TextureHandle source,
                                      TextureHandle destination,
                                      std::span<BlitRegion> regions,
                                      Filter filter) {
  VkImage srcImage = mRegistry.getTextures().at(source)->getImage();
  VkImage dstImage = mRegistry.getTextures().at(destination)->getImage();

  static constexpr usize OffsetSize = 2;
  std::vector<VkImageBlit> vkRegions(regions.size());
  for (usize i = 0; i < regions.size(); ++i) {
    auto &region = regions[i];
    auto &vkRegion = vkRegions.at(i);

    vkRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vkRegion.srcSubresource.mipLevel = region.srcMipLevel;
    vkRegion.srcSubresource.baseArrayLayer = region.srcBaseArrayLayer;
    vkRegion.srcSubresource.layerCount = region.srcLayerCount;

    for (usize i = 0; i < OffsetSize; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      vkRegion.srcOffsets[i] = {region.srcOffsets.at(i).x,
                                region.srcOffsets.at(i).y,
                                region.srcOffsets.at(i).z};
    }

    vkRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vkRegion.dstSubresource.mipLevel = region.dstMipLevel;
    vkRegion.dstSubresource.baseArrayLayer = region.dstBaseArrayLayer;
    vkRegion.dstSubresource.layerCount = region.dstLayerCount;
    for (usize i = 0; i < OffsetSize; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      vkRegion.dstOffsets[i] = {region.dstOffsets.at(i).x,
                                region.dstOffsets.at(i).y,
                                region.dstOffsets.at(i).z};
    }
  }

  vkCmdBlitImage(mCommandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                 dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                 static_cast<u32>(regions.size()), vkRegions.data(),
                 VulkanMapping::getFilter(filter));
}

void VulkanCommandBuffer::writeTimestamp(u32 queryIndex, PipelineStage stage) {
  vkCmdWriteTimestamp2KHR(mCommandBuffer,
                          VulkanMapping::getPipelineStageFlags(stage),
                          mTimestampManager.getQueryPool(), queryIndex);
}

} // namespace quoll::rhi

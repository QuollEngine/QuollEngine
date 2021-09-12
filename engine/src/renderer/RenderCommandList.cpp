#include "RenderCommandList.h"

namespace liquid {

void RenderCommandList::beginRenderPass(
    VkRenderPass renderPass, VkFramebuffer framebuffer,
    const glm::ivec2 &renderAreaOffset, const glm::uvec2 &renderAreaSize,
    const std::vector<VkClearValue> &clearValues) {
  auto *command = new RenderCommandBeginRenderPass;
  command->renderPass = renderPass;
  command->framebuffer = framebuffer;
  command->renderAreaOffset = renderAreaOffset;
  command->renderAreaSize = renderAreaSize;
  command->clearValues = clearValues;
  record(command);
}

void RenderCommandList::endRenderPass() {
  record(new RenderCommandEndRenderPass);
}

void RenderCommandList::bindDescriptorSets(
    VkPipelineLayout pipelineLayout, VkPipelineBindPoint bindPoint,
    uint32_t firstSet, const std::vector<VkDescriptorSet> &descriptorSets,
    const std::vector<uint32_t> &dynamicOffsets) {
  auto *command = new RenderCommandBindDescriptorSets;
  command->pipelineLayout = pipelineLayout;
  command->bindPoint = bindPoint;
  command->firstSet = firstSet;
  command->descriptorSets = descriptorSets;
  command->dynamicOffsets = dynamicOffsets;
  record(command);
}

void RenderCommandList::bindPipeline(VkPipeline pipeline,
                                     VkPipelineBindPoint bindPoint) {
  auto *command = new RenderCommandBindPipeline;
  command->pipeline = pipeline;
  command->bindPoint = bindPoint;
  record(command);
}

void RenderCommandList::bindVertexBuffer(
    const SharedPtr<HardwareBuffer> &buffer) {
  LIQUID_ASSERT(buffer->getType() == HardwareBuffer::Vertex,
                "Passed hardware buffer must be vertex buffer");
  auto *command = new RenderCommandBindVertexBuffer;
  command->buffer = buffer;
  record(command);
}

void RenderCommandList::bindIndexBuffer(const SharedPtr<HardwareBuffer> &buffer,
                                        VkIndexType indexType) {
  LIQUID_ASSERT(buffer->getType() == HardwareBuffer::Index,
                "Passed hardware buffer must be index buffer");
  auto *command = new RenderCommandBindIndexBuffer;
  command->buffer = buffer;
  command->indexType = indexType;
  record(command);
}

void RenderCommandList::pushConstants(VkPipelineLayout pipelineLayout,
                                      VkShaderStageFlags stageFlags,
                                      uint32_t offset, uint32_t size,
                                      void *data) {
  auto *command = new RenderCommandPushConstants;
  command->pipelineLayout = pipelineLayout;
  command->stageFlags = stageFlags;
  command->offset = offset;
  command->size = size;
  command->data.reset(static_cast<char *>(data));
  record(command);
}

void RenderCommandList::setViewport(const glm::vec2 &offset,
                                    const glm::vec2 &size,
                                    const glm::vec2 &depthRange) {
  auto *command = new RenderCommandSetViewport;
  command->offset = offset;
  command->size = size;
  command->depthRange = depthRange;
  record(command);
}

void RenderCommandList::drawIndexed(size_t indexCount, uint32_t firstIndex,
                                    int32_t vertexOffset) {
  auto *command = new RenderCommandDrawIndexed;
  command->indexCount = indexCount;
  command->firstIndex = firstIndex;
  command->vertexOffset = vertexOffset;
  record(command);
}

void RenderCommandList::setScissor(const glm::vec2 &offset,
                                   const glm::vec2 &size) {
  auto *command = new RenderCommandSetScissor;
  command->offset = offset;
  command->size = size;
  record(command);
}

void RenderCommandList::record(RenderCommandBase *command) {
  commands.push_back(std::unique_ptr<RenderCommandBase>(command));
}

} // namespace liquid

#include "liquid/core/Base.h"
#include "MockCommandList.h"

namespace liquid::rhi {

template <class T>
static constexpr std::vector<T> vectorFrom(std::span<T> data) {
  return std::vector<T>(data.begin(), data.end());
}

MockCommandList::MockCommandList(MockCommandList &&rhs) {
  mCommands = std::move(rhs.mCommands);
  mDrawCalls = std::move(rhs.mDrawCalls);
  mDispatchCalls = std::move(rhs.mDispatchCalls);
}

void MockCommandList::beginRenderPass(rhi::RenderPassHandle renderPass,
                                      FramebufferHandle framebuffer,
                                      const glm::ivec2 &renderAreaOffset,
                                      const glm::uvec2 &renderAreaSize) {
  auto *command = new MockCommandBeginRenderPass;
  command->renderPass = renderPass;
  command->framebuffer = framebuffer;
  command->renderAreaOffset = renderAreaOffset;
  command->renderAreaSize = renderAreaSize;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));

  mBindings.renderPass = renderPass;
}

void MockCommandList::endRenderPass() {
  mCommands.push_back(
      std::unique_ptr<MockCommand>(new MockCommandBeginRenderPass));

  mBindings.renderPass = RenderPassHandle::Null;
}

void MockCommandList::bindPipeline(PipelineHandle pipeline) {
  auto *command = new MockCommandBindPipeline;
  command->pipeline = pipeline;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));

  mBindings.pipeline = pipeline;
}

void MockCommandList::bindDescriptor(PipelineHandle pipeline, uint32_t firstSet,
                                     const Descriptor &descriptor,
                                     std::span<uint32_t> dynamicOffsets) {
  auto *command = new MockCommandBindDescriptor;
  command->pipeline = pipeline;
  command->firstSet = firstSet;
  command->descriptor = descriptor;
  command->dynamicOffsets =
      std::vector(dynamicOffsets.begin(), dynamicOffsets.end());
  mCommands.push_back(std::unique_ptr<MockCommand>(command));

  mBindings.descriptors.insert_or_assign(firstSet, descriptor);
}

void MockCommandList::bindVertexBuffer(BufferHandle buffer) {
  auto *command = new MockCommandBindVertexBuffer;
  command->buffer = buffer;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));

  mBindings.vertexBuffer = buffer;
}

void MockCommandList::bindIndexBuffer(BufferHandle buffer,
                                      IndexType indexType) {
  auto *command = new MockCommandBindIndexBuffer;
  command->buffer = buffer;
  command->indexType = indexType;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));

  mBindings.indexBuffer = buffer;
  mBindings.indexType = indexType;
}

void MockCommandList::pushConstants(PipelineHandle pipeline,
                                    ShaderStage shaderStage, uint32_t offset,
                                    uint32_t size, void *data) {
  auto *command = new MockCommandPushConstants;
  command->pipeline = pipeline;
  command->shaderStage = shaderStage;
  command->offset = offset;
  command->size = size;
  command->data = data;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));
}

void MockCommandList::draw(uint32_t vertexCount, uint32_t firstVertex,
                           uint32_t instanceCount, uint32_t firstInstance) {
  auto *command = new MockCommandDraw;
  command->vertexCount = vertexCount;
  command->firstVertex = firstVertex;
  command->instanceCount = instanceCount;
  command->firstInstance = firstInstance;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));

  MockDrawCall call{};
  call.type = DrawCallType::Draw;
  call.bindings = mBindings;
  call.command = command;
  mDrawCalls.push_back(call);
}

void MockCommandList::drawIndexed(uint32_t indexCount, uint32_t firstIndex,
                                  int32_t vertexOffset, uint32_t instanceCount,
                                  uint32_t firstInstance) {
  auto *command = new MockCommandDrawIndexed;
  command->indexCount = indexCount;
  command->firstIndex = firstIndex;
  command->vertexOffset = vertexOffset;
  command->instanceCount = instanceCount;
  command->firstInstance = firstInstance;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));

  MockDrawCall call{};
  call.type = DrawCallType::DrawIndexed;
  call.bindings = mBindings;
  call.command = command;
  mDrawCalls.push_back(call);
}

void MockCommandList::dispatch(uint32_t groupCountX, uint32_t groupCountY,
                               uint32_t groupCountZ) {
  auto *command = new MockCommandDispatch;
  command->groupCountX = groupCountX;
  command->groupCountY = groupCountY;
  command->groupCountZ = groupCountZ;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));

  mDispatchCalls.push_back({mBindings, groupCountX, groupCountY, groupCountZ});
}

void MockCommandList::setViewport(const glm::vec2 &offset,
                                  const glm::vec2 &size,
                                  const glm::vec2 &depthRange) {
  auto *command = new MockCommandSetViewport;
  command->offset = offset;
  command->size = size;
  command->depthRange = depthRange;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));
}

void MockCommandList::setScissor(const glm::ivec2 &offset,
                                 const glm::uvec2 &size) {
  auto *command = new MockCommandSetScissor;
  command->offset = offset;
  command->size = size;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));
}

void MockCommandList::pipelineBarrier(PipelineStage srcStage,
                                      PipelineStage dstStage,
                                      std::span<MemoryBarrier> memoryBarriers,
                                      std::span<ImageBarrier> imageBarriers) {
  auto *command = new MockCommandPipelineBarrier;
  command->srcStage = dstStage;
  command->memoryBarriers = vectorFrom(memoryBarriers);
  command->imageBarriers = vectorFrom(imageBarriers);
  mCommands.push_back(std::unique_ptr<MockCommand>(command));
}

void MockCommandList::copyTextureToBuffer(TextureHandle srcTexture,
                                          BufferHandle dstBuffer,
                                          std::span<CopyRegion> copyRegions) {
  auto *command = new MockCommandCopyTextureToBuffer;
  command->srcTexture = srcTexture;
  command->dstBuffer = dstBuffer;
  command->copyRegions = vectorFrom(copyRegions);
  mCommands.push_back(std::unique_ptr<MockCommand>(command));
}

void MockCommandList::copyBufferToTexture(BufferHandle srcBuffer,
                                          TextureHandle dstTexture,
                                          std::span<CopyRegion> copyRegions) {
  auto *command = new MockCommandCopyBufferToTexture;
  command->srcBuffer = srcBuffer;
  command->dstTexture = dstTexture;
  command->copyRegions = vectorFrom(copyRegions);
  mCommands.push_back(std::unique_ptr<MockCommand>(command));
}

void MockCommandList::blitTexture(TextureHandle source,
                                  TextureHandle destination,
                                  std::span<BlitRegion> regions,
                                  Filter filter) {
  auto *command = new MockCommandBlitTexture;
  command->source = source;
  command->destination = destination;
  command->regions = vectorFrom(regions);
  mCommands.push_back(std::unique_ptr<MockCommand>(command));
}

void MockCommandList::clear() {
  mBindings = MockBindings{};
  mDrawCalls.clear();
  mDispatchCalls.clear();
  mCommands.clear();
}

} // namespace liquid::rhi

#include "quoll/core/Base.h"
#include "MockCommandList.h"

namespace quoll::rhi {

template <class T> constexpr std::vector<T> vectorFrom(std::span<T> data) {
  return std::vector<T>(data.begin(), data.end());
}

MockCommandList::MockCommandList(MockCommandList &&rhs) noexcept {
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

void MockCommandList::bindDescriptor(PipelineHandle pipeline, u32 firstSet,
                                     const Descriptor &descriptor,
                                     std::span<u32> dynamicOffsets) {
  auto *command = new MockCommandBindDescriptor;
  command->pipeline = pipeline;
  command->firstSet = firstSet;
  command->descriptor = descriptor;
  command->dynamicOffsets =
      std::vector(dynamicOffsets.begin(), dynamicOffsets.end());
  mCommands.push_back(std::unique_ptr<MockCommand>(command));

  mBindings.descriptors.insert_or_assign(firstSet, descriptor);
}

void MockCommandList::bindVertexBuffers(
    const std::span<const BufferHandle> buffers,
    const std::span<const u64> offsets) {
  auto *command = new MockCommandBindVertexBuffer;
  command->buffers = std::vector(buffers.begin(), buffers.end());
  command->offsets = std::vector(offsets.begin(), offsets.end());
  mCommands.push_back(std::unique_ptr<MockCommand>(command));

  mBindings.vertexBuffers = command->buffers;
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
                                    ShaderStage shaderStage, u32 offset,
                                    u32 size, void *data) {
  auto *command = new MockCommandPushConstants;
  command->pipeline = pipeline;
  command->shaderStage = shaderStage;
  command->offset = offset;
  command->size = size;
  command->data = data;
  mCommands.push_back(std::unique_ptr<MockCommand>(command));
}

void MockCommandList::draw(u32 vertexCount, u32 firstVertex, u32 instanceCount,
                           u32 firstInstance) {
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

void MockCommandList::drawIndexed(u32 indexCount, u32 firstIndex,
                                  i32 vertexOffset, u32 instanceCount,
                                  u32 firstInstance) {
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

void MockCommandList::dispatch(u32 groupCountX, u32 groupCountY,
                               u32 groupCountZ) {
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

void MockCommandList::pipelineBarrier(std::span<MemoryBarrier> memoryBarriers,
                                      std::span<ImageBarrier> imageBarriers,
                                      std::span<BufferBarrier> bufferBarriers) {
  auto *command = new MockCommandPipelineBarrier;
  command->memoryBarriers = vectorFrom(memoryBarriers);
  command->imageBarriers = vectorFrom(imageBarriers);
  command->bufferBarriers = vectorFrom(bufferBarriers);
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

void MockCommandList::writeTimestamp(u32 queryIndex, rhi::PipelineStage stage) {
  auto *command = new MockCommandTimestamp;
  command->queryIndex = queryIndex;
  command->stage = stage;

  mCommands.push_back(std::unique_ptr<MockCommand>(command));
}

} // namespace quoll::rhi

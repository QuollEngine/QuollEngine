#include "quoll/core/Base.h"
#include "RenderGraph.h"
#include "RenderGraphPass.h"

namespace quoll {

RenderGraphPass::RenderGraphPass(StringView name, RenderGraphPassType type)
    : mName(name), mType(type) {}

void RenderGraphPass::write(RenderGraphResource<rhi::TextureHandle> handle,
                            AttachmentType type,
                            const rhi::AttachmentClearValue &clearValue) {
  mTextureOutputs.push_back({handle});
  mAttachments.push_back({type, clearValue});
}

void RenderGraphPass::read(RenderGraphResource<rhi::TextureHandle> handle) {
  mTextureInputs.push_back({handle});
}

void RenderGraphPass::write(rhi::BufferHandle handle, rhi::BufferUsage usage) {
  QuollAssert(
      !BitwiseEnumContains(usage, quoll::rhi::BufferUsage::Vertex) &&
          !BitwiseEnumContains(usage, quoll::rhi::BufferUsage::Index) &&
          !BitwiseEnumContains(usage, quoll::rhi::BufferUsage::Indirect),
      "Buffers can only be written from Uniform or Storage");
  mBufferOutputs.push_back({handle, usage});
}

void RenderGraphPass::read(rhi::BufferHandle handle, rhi::BufferUsage usage) {
  if (mType == RenderGraphPassType::Compute) {
    QuollAssert(!BitwiseEnumContains(usage, quoll::rhi::BufferUsage::Vertex) &&
                    !BitwiseEnumContains(usage, quoll::rhi::BufferUsage::Index),
                "Compute pass can only read "
                "buffers from uniform, storage, "
                "or indirect");
  }
  mBufferInputs.push_back({handle, usage});
}

void RenderGraphPass::setExecutor(const ExecutorFn &executor) {
  mExecutor = executor;
}

void RenderGraphPass::addPipeline(rhi::PipelineHandle handle) {
  mPipelines.push_back(handle);
}

void RenderGraphPass::execute(rhi::RenderCommandList &commandList,
                              u32 frameIndex) {
  mExecutor(commandList, frameIndex);
}

} // namespace quoll

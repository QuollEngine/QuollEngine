#include "liquid/core/Base.h"
#include "RenderGraphPass.h"

namespace liquid {

RenderGraphPass::RenderGraphPass(StringView name) : mName(name) {}

void RenderGraphPass::write(rhi::TextureHandle handle,
                            const rhi::AttachmentClearValue &clearValue) {
  mOutputs.push_back({handle});
  mAttachments.push_back({clearValue});
}

void RenderGraphPass::read(rhi::TextureHandle handle) {
  mInputs.push_back({handle});
}

void RenderGraphPass::setExecutor(const ExecutorFn &executor) {
  mExecutor = executor;
}

VirtualPipelineHandle
RenderGraphPass::addPipeline(const rhi::PipelineDescription &description) {
  return mRegistry.set(description);
}

void RenderGraphPass::execute(rhi::RenderCommandList &commandList,
                              uint32_t frameIndex) {
  mExecutor(commandList, mRegistry, frameIndex);
}

} // namespace liquid

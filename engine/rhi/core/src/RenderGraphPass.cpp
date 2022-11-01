#include "liquid/core/Base.h"
#include "RenderGraphPass.h"

namespace liquid::rhi {

RenderGraphPass::RenderGraphPass(StringView name) : mName(name) {}

void RenderGraphPass::write(TextureHandle handle,
                            const AttachmentClearValue &clearValue) {
  mOutputs.push_back({handle});
  mAttachments.push_back({clearValue});
}

void RenderGraphPass::read(TextureHandle handle) {
  mInputs.push_back({handle});
}

void RenderGraphPass::setExecutor(const ExecutorFn &executor) {
  mExecutor = executor;
}

VirtualPipelineHandle
RenderGraphPass::addPipeline(const PipelineDescription &description) {
  return mRegistry.set(description);
}

void RenderGraphPass::execute(RenderCommandList &commandList,
                              uint32_t frameIndex) {
  mExecutor(commandList, mRegistry, frameIndex);
}

} // namespace liquid::rhi

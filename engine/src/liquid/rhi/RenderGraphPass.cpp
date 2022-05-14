#include "liquid/core/Base.h"
#include "RenderGraphPass.h"

namespace liquid::rhi {

RenderGraphPass::RenderGraphPass(StringView name) : mName(name) {}

void RenderGraphPass::write(TextureHandle handle,
                            const AttachmentClearValue &clearValue) {
  mOutputs.push_back(handle);
  mAttachments.push_back({clearValue});
}

void RenderGraphPass::read(TextureHandle handle) { mInputs.push_back(handle); }

void RenderGraphPass::setExecutor(const ExecutorFn &executor) {
  mExecutor = executor;
}

void RenderGraphPass::addPipeline(PipelineHandle handle) {
  mPipelines.push_back(handle);
}

void RenderGraphPass::execute(RenderCommandList &commandList) {
  mExecutor(commandList);
}

} // namespace liquid::rhi

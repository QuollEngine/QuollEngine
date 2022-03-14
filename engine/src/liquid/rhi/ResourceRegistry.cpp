#include "liquid/core/Base.h"
#include "ResourceRegistry.h"

namespace liquid::rhi {

ShaderHandle ResourceRegistry::addShader(const ShaderDescription &description) {
  return mShaders.addDescription(description);
}

void ResourceRegistry::deleteShader(ShaderHandle handle) {
  mShaders.deleteDescription(handle);
}

BufferHandle ResourceRegistry::addBuffer(const BufferDescription &description) {
  return mBuffers.addDescription(description);
}

void ResourceRegistry::deleteBuffer(BufferHandle handle) {
  mBuffers.deleteDescription(handle);
}

void ResourceRegistry::updateBuffer(BufferHandle handle,
                                    const BufferDescription &description) {
  LIQUID_ASSERT(handle > 0, "Buffer does not exist");
  mBuffers.updateDescription(handle, description);
}

TextureHandle
ResourceRegistry::addTexture(const TextureDescription &description) {
  return mTextures.addDescription(description);
}

void ResourceRegistry::deleteTexture(TextureHandle handle) {
  mTextures.deleteDescription(handle);
}

rhi::RenderPassHandle
ResourceRegistry::addRenderPass(const RenderPassDescription &description) {
  return mRenderPasses.addDescription(description);
}

void ResourceRegistry::updateRenderPass(
    rhi::RenderPassHandle handle, const RenderPassDescription &description) {
  LIQUID_ASSERT(handle > 0, "Render pass does not exist");
  mRenderPasses.updateDescription(handle, description);
}

void ResourceRegistry::deleteRenderPass(rhi::RenderPassHandle handle) {
  mRenderPasses.deleteDescription(handle);
}

FramebufferHandle
ResourceRegistry::addFramebuffer(const FramebufferDescription &description) {
  return mFramebuffers.addDescription(description);
}

void ResourceRegistry::updateFramebuffer(
    FramebufferHandle handle, const FramebufferDescription &description) {
  LIQUID_ASSERT(handle > 0, "Framebuffer does not exist");
  mFramebuffers.updateDescription(handle, description);
}

void ResourceRegistry::deleteFramebuffer(FramebufferHandle handle) {
  mFramebuffers.deleteDescription(handle);
}

PipelineHandle
ResourceRegistry::addPipeline(const PipelineDescription &description) {
  return mPipelines.addDescription(description);
}

void ResourceRegistry::updatePipeline(PipelineHandle handle,
                                      const PipelineDescription &description) {
  LIQUID_ASSERT(handle > 0, "Pipeline does not exist");
  mPipelines.updateDescription(handle, description);
}

void ResourceRegistry::deletePipeline(PipelineHandle handle) {
  mPipelines.deleteDescription(handle);
}

} // namespace liquid::rhi

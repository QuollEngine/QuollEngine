#include "liquid/core/Base.h"
#include "ResourceRegistry.h"

namespace liquid::rhi {

ShaderHandle ResourceRegistry::setShader(const ShaderDescription &description,
                                         ShaderHandle handle) {
  return mShaders.setDescription(description, handle);
}

void ResourceRegistry::deleteShader(ShaderHandle handle) {
  mShaders.deleteDescription(handle);
}

BufferHandle ResourceRegistry::setBuffer(const BufferDescription &description,
                                         BufferHandle handle) {
  return mBuffers.setDescription(description, handle);
}

void ResourceRegistry::deleteBuffer(BufferHandle handle) {
  mBuffers.deleteDescription(handle);
}

TextureHandle
ResourceRegistry::setTexture(const TextureDescription &description,
                             TextureHandle handle) {
  return mTextures.setDescription(description, handle);
}

void ResourceRegistry::deleteTexture(TextureHandle handle) {
  mTextures.deleteDescription(handle);
}

RenderPassHandle
ResourceRegistry::setRenderPass(const RenderPassDescription &description,
                                RenderPassHandle handle) {
  return mRenderPasses.setDescription(description, handle);
}

void ResourceRegistry::deleteRenderPass(rhi::RenderPassHandle handle) {
  mRenderPasses.deleteDescription(handle);
}

FramebufferHandle
ResourceRegistry::setFramebuffer(const FramebufferDescription &description,
                                 FramebufferHandle handle) {
  return mFramebuffers.setDescription(description, handle);
}

void ResourceRegistry::deleteFramebuffer(FramebufferHandle handle) {
  mFramebuffers.deleteDescription(handle);
}

PipelineHandle
ResourceRegistry::setPipeline(const PipelineDescription &description,
                              PipelineHandle handle) {
  return mPipelines.setDescription(description, handle);
}

void ResourceRegistry::deletePipeline(PipelineHandle handle) {
  mPipelines.deleteDescription(handle);
}

} // namespace liquid::rhi

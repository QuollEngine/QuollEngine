#include "liquid/core/Base.h"

#include "VulkanResourceRegistry.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"

namespace liquid::rhi {

void VulkanResourceRegistry::setShader(std::unique_ptr<VulkanShader> &&shader,
                                       ShaderHandle handle) {
  mShaders.map.insert_or_assign(handle, std::move(shader));
}

void VulkanResourceRegistry::deleteShader(ShaderHandle handle) {
  mShaders.map.erase(handle);
}

BufferHandle
VulkanResourceRegistry::setBuffer(std::unique_ptr<VulkanBuffer> &&buffer) {
  auto handle = BufferHandle{mBuffers.lastHandle};
  mBuffers.lastHandle++;
  mBuffers.map.insert_or_assign(handle, std::move(buffer));

  return handle;
}

void VulkanResourceRegistry::deleteBuffer(BufferHandle handle) {
  mBuffers.map.erase(handle);
}

void VulkanResourceRegistry::setTexture(
    std::unique_ptr<VulkanTexture> &&texture, TextureHandle handle) {
  mTextures.map.insert_or_assign(handle, std::move(texture));
}

void VulkanResourceRegistry::deleteTexture(TextureHandle handle) {
  mTextures.map.erase(handle);
}

void VulkanResourceRegistry::setRenderPass(
    std::unique_ptr<VulkanRenderPass> &&renderPass, RenderPassHandle handle) {
  mRenderPasses.map.insert_or_assign(handle, std::move(renderPass));
}

void VulkanResourceRegistry::deleteRenderPass(RenderPassHandle handle) {
  mRenderPasses.map.erase(handle);
}

void VulkanResourceRegistry::setFramebuffer(
    std::unique_ptr<VulkanFramebuffer> &&framebuffer,
    FramebufferHandle handle) {
  mFramebuffers.map.insert_or_assign(handle, std::move(framebuffer));
}

void VulkanResourceRegistry::deleteFramebuffer(FramebufferHandle handle) {
  mFramebuffers.map.erase(handle);
}

void VulkanResourceRegistry::setPipeline(
    std::unique_ptr<VulkanPipeline> &&pipeline, PipelineHandle handle) {
  mPipelines.map.insert_or_assign(handle, std::move(pipeline));
}

void VulkanResourceRegistry::deletePipeline(PipelineHandle handle) {
  mPipelines.map.erase(handle);
}

} // namespace liquid::rhi

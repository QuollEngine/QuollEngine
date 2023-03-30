#include "liquid/core/Base.h"

#include "VulkanResourceRegistry.h"
#include "VulkanTexture.h"
#include "VulkanTextureView.h"
#include "VulkanBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"

namespace liquid::rhi {

ShaderHandle
VulkanResourceRegistry::setShader(std::unique_ptr<VulkanShader> &&shader) {
  auto handle = ShaderHandle{mShaders.lastHandle};
  mShaders.lastHandle++;

  mShaders.map.insert_or_assign(handle, std::move(shader));
  return handle;
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

TextureHandle
VulkanResourceRegistry::setTexture(std::unique_ptr<VulkanTexture> &&texture) {
  auto handle = TextureHandle{mTextures.lastHandle};
  mTextures.lastHandle++;
  mTextures.map.insert_or_assign(handle, std::move(texture));
  return handle;
}

void VulkanResourceRegistry::recreateTexture(
    TextureHandle handle, std::unique_ptr<VulkanTexture> &&texture) {
  mTextures.map.insert_or_assign(handle, std::move(texture));
}

void VulkanResourceRegistry::deleteTexture(TextureHandle handle) {
  mTextures.map.erase(handle);
}

TextureViewHandle VulkanResourceRegistry::setTextureView(
    std::unique_ptr<VulkanTextureView> &&textureView) {
  auto handle = TextureViewHandle{mTextureViews.lastHandle};
  mTextureViews.lastHandle++;
  mTextureViews.map.insert_or_assign(handle, std::move(textureView));

  return handle;
}

void VulkanResourceRegistry::deleteTextureView(TextureViewHandle handle) {
  mTextureViews.map.erase(handle);
}

RenderPassHandle VulkanResourceRegistry::setRenderPass(
    std::unique_ptr<VulkanRenderPass> &&renderPass) {
  auto handle = RenderPassHandle{mRenderPasses.lastHandle};
  mRenderPasses.lastHandle++;
  mRenderPasses.map.insert_or_assign(handle, std::move(renderPass));

  return handle;
}

void VulkanResourceRegistry::deleteRenderPass(RenderPassHandle handle) {
  mRenderPasses.map.erase(handle);
}

FramebufferHandle VulkanResourceRegistry::setFramebuffer(
    std::unique_ptr<VulkanFramebuffer> &&framebuffer) {
  auto handle = FramebufferHandle{mFramebuffers.lastHandle};
  mFramebuffers.lastHandle++;
  mFramebuffers.map.insert_or_assign(handle, std::move(framebuffer));

  return handle;
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

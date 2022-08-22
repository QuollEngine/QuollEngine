#include "liquid/core/Base.h"
#include "VulkanResourceRegistry.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"

namespace liquid::rhi {

ShaderHandle
VulkanResourceRegistry::setShader(std::unique_ptr<VulkanShader> &&shader) {
  auto handle = ShaderHandle{mBuffers.lastHandle};
  mBuffers.lastHandle++;

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

  if (texture->isFramebufferRelative()) {
    mSwapchainRelativeTextures.insert(handle);
  }

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

void VulkanResourceRegistry::deleteDanglingSwapchainRelativeTextures() {
  for (auto it = mSwapchainRelativeTextures.begin();
       it != mSwapchainRelativeTextures.end(); ++it) {
    auto textureIt = mTextures.map.find(*it);
    if (textureIt == mTextures.map.end() ||
        !textureIt->second->isFramebufferRelative()) {
      it = mSwapchainRelativeTextures.erase(it);
    }
  }
}

void VulkanResourceRegistry::setRenderPass(
    rhi::RenderPassHandle handle,
    std::unique_ptr<VulkanRenderPass> &&renderPass) {
  mRenderPasses.insert_or_assign(handle, std::move(renderPass));
}

void VulkanResourceRegistry::deleteRenderPass(RenderPassHandle handle) {
  mRenderPasses.erase(handle);
}

void VulkanResourceRegistry::setFramebuffer(
    FramebufferHandle handle,
    std::unique_ptr<VulkanFramebuffer> &&framebuffer) {
  mFramebuffers.insert_or_assign(handle, std::move(framebuffer));
}

void VulkanResourceRegistry::deleteFramebuffer(FramebufferHandle handle) {
  mFramebuffers.erase(handle);
}

void VulkanResourceRegistry::setPipeline(
    PipelineHandle handle, std::unique_ptr<VulkanPipeline> &&pipeline) {
  mPipelines.insert_or_assign(handle, std::move(pipeline));
}

void VulkanResourceRegistry::deletePipeline(PipelineHandle handle) {
  mPipelines.erase(handle);
}

} // namespace liquid::rhi

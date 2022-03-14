#include "liquid/core/Base.h"
#include "VulkanResourceRegistry.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"

namespace liquid::experimental {

void VulkanResourceRegistry::addShader(ShaderHandle handle,
                                       std::unique_ptr<VulkanShader> &&shader) {
  mShaders.insert({handle, std::move(shader)});
}

void VulkanResourceRegistry::removeShader(ShaderHandle handle) {
  mShaders.erase(handle);
}

void VulkanResourceRegistry::addBuffer(BufferHandle handle,
                                       std::unique_ptr<VulkanBuffer> &&buffer) {
  mBuffers.insert({handle, std::move(buffer)});
}

void VulkanResourceRegistry::removeBuffer(BufferHandle handle) {
  mBuffers.erase(handle);
}

void VulkanResourceRegistry::updateBuffer(
    BufferHandle handle, std::unique_ptr<VulkanBuffer> &&buffer) {
  mBuffers.at(handle) = std::move(buffer);
}

void VulkanResourceRegistry::addTexture(
    TextureHandle handle, std::unique_ptr<VulkanTexture> &&texture) {
  mTextures.insert({handle, std::move(texture)});
}

void VulkanResourceRegistry::removeTexture(TextureHandle handle) {
  mTextures.erase(handle);
}

void VulkanResourceRegistry::updateTexture(
    TextureHandle handle, std::unique_ptr<VulkanTexture> &&texture) {
  mTextures.at(handle) = std::move(texture);
}

void VulkanResourceRegistry::addRenderPass(
    RenderPassHandle handle, std::unique_ptr<VulkanRenderPass> &&renderPass) {
  mRenderPasses.insert({handle, std::move(renderPass)});
}

void VulkanResourceRegistry::updateRenderPass(
    RenderPassHandle handle, std::unique_ptr<VulkanRenderPass> &&renderPass) {
  mRenderPasses.at(handle) = std::move(renderPass);
}

void VulkanResourceRegistry::addFramebuffer(
    FramebufferHandle handle,
    std::unique_ptr<VulkanFramebuffer> &&framebuffer) {
  mFramebuffers.insert({handle, std::move(framebuffer)});
}

void VulkanResourceRegistry::updateFramebuffer(
    RenderPassHandle handle, std::unique_ptr<VulkanFramebuffer> &&framebuffer) {
  mFramebuffers.at(handle) = std::move(framebuffer);
}

void VulkanResourceRegistry::addPipeline(
    PipelineHandle handle, std::unique_ptr<VulkanPipeline> &&pipeline) {
  mPipelines.insert({handle, std::move(pipeline)});
}

void VulkanResourceRegistry::updatePipeline(
    RenderPassHandle handle, std::unique_ptr<VulkanPipeline> &&pipeline) {
  mPipelines.at(handle) = std::move(pipeline);
}

} // namespace liquid::experimental

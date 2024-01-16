#pragma once

#include "quoll/rhi/RenderHandle.h"

namespace quoll::rhi {

class VulkanBuffer;
class VulkanTexture;
class VulkanTextureView;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanPipeline;
class VulkanShader;
class VulkanSampler;

class VulkanResourceRegistry {

  template <class THandle, class TResource> struct ResourceMap {
    using Map = std::unordered_map<THandle, std::unique_ptr<TResource>>;

    Map map;

    u32 lastHandle = 1;
  };

  using ShaderMap = ResourceMap<ShaderHandle, VulkanShader>;
  using BufferMap = ResourceMap<BufferHandle, VulkanBuffer>;
  using TextureMap = ResourceMap<TextureHandle, VulkanTexture>;
  using SamplerMap = ResourceMap<SamplerHandle, VulkanSampler>;
  using RenderPassMap = ResourceMap<RenderPassHandle, VulkanRenderPass>;
  using FramebufferMap = ResourceMap<FramebufferHandle, VulkanFramebuffer>;
  using PipelineMap = ResourceMap<PipelineHandle, VulkanPipeline>;

public:
  void setShader(std::unique_ptr<VulkanShader> &&shader, ShaderHandle handle);

  void deleteShader(ShaderHandle handle);

  inline const ShaderMap::Map &getShaders() const { return mShaders.map; }

  BufferHandle setBuffer(std::unique_ptr<VulkanBuffer> &&buffer);

  void deleteBuffer(BufferHandle handle);

  inline bool hasBuffer(BufferHandle handle) const {
    return mBuffers.map.find(handle) != mBuffers.map.end();
  }

  inline const BufferMap::Map &getBuffers() const { return mBuffers.map; }

  void setTexture(std::unique_ptr<VulkanTexture> &&texture,
                  TextureHandle handle);

  void deleteTexture(TextureHandle handle);

  inline const TextureMap::Map &getTextures() const { return mTextures.map; }

  void setSampler(std::unique_ptr<VulkanSampler> &&sampler,
                  SamplerHandle handle);

  void deleteSampler(SamplerHandle handle);

  inline const SamplerMap::Map &getSamplers() const { return mSamplers.map; }

  void setRenderPass(std::unique_ptr<VulkanRenderPass> &&renderPass,
                     RenderPassHandle handle);

  void deleteRenderPass(rhi::RenderPassHandle handle);

  inline const RenderPassMap::Map &getRenderPasses() const {
    return mRenderPasses.map;
  }

  void setFramebuffer(std::unique_ptr<VulkanFramebuffer> &&framebuffer,
                      FramebufferHandle handle);

  void deleteFramebuffer(rhi::FramebufferHandle handle);

  inline const FramebufferMap::Map &getFramebuffers() const {
    return mFramebuffers.map;
  }

  void setPipeline(std::unique_ptr<VulkanPipeline> &&pipeline,
                   PipelineHandle handle);

  void deletePipeline(PipelineHandle handle);

  inline bool hasPipeline(PipelineHandle handle) const {
    return mPipelines.map.find(handle) != mPipelines.map.end();
  }

  inline const PipelineMap::Map &getPipelines() const { return mPipelines.map; }

private:
  BufferMap mBuffers;
  TextureMap mTextures;
  SamplerMap mSamplers;
  ShaderMap mShaders;
  RenderPassMap mRenderPasses;
  FramebufferMap mFramebuffers;
  PipelineMap mPipelines;
};

} // namespace quoll::rhi

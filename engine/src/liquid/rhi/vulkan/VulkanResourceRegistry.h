#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid::rhi {

class VulkanBuffer;
class VulkanTexture;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanPipeline;
class VulkanShader;

class VulkanResourceRegistry {
  template <class THandle, class TVulkanObject>
  using VulkanResourceMap =
      std::unordered_map<THandle, std::unique_ptr<TVulkanObject>>;

  using ShaderMap = VulkanResourceMap<ShaderHandle, VulkanShader>;
  using BufferMap = VulkanResourceMap<BufferHandle, VulkanBuffer>;
  using TextureMap = VulkanResourceMap<TextureHandle, VulkanTexture>;
  using RenderPassMap =
      VulkanResourceMap<rhi::RenderPassHandle, VulkanRenderPass>;
  using FramebufferMap =
      VulkanResourceMap<FramebufferHandle, VulkanFramebuffer>;
  using PipelineMap = VulkanResourceMap<PipelineHandle, VulkanPipeline>;

public:
  /**
   * @brief Add shader
   *
   * @param handle Shader handle
   * @param shader Vulkan shader
   */
  void addShader(ShaderHandle handle, std::unique_ptr<VulkanShader> &&shader);

  /**
   * @brief Remove shader
   *
   * @param handle Shader handle
   */
  void removeShader(ShaderHandle handle);

  /**
   * @brief Get shader
   *
   * @param handle Shader handle
   * @return Vulkan shader
   */
  inline const std::unique_ptr<VulkanShader> &
  getShader(ShaderHandle handle) const {
    return mShaders.at(handle);
  }

  /**
   * @brief Add buffer
   *
   * @param handle Buffer handle
   * @param buffer Vulkan buffer
   */
  void addBuffer(BufferHandle handle, std::unique_ptr<VulkanBuffer> &&buffer);

  /**
   * @brief Remove buffer
   *
   * @param handle Buffer handle
   */
  void removeBuffer(BufferHandle handle);

  /**
   * @brief Update buffer
   *
   * @param handle Buffer handle
   * @param buffer Vulkan buffer
   */
  void updateBuffer(BufferHandle handle,
                    std::unique_ptr<VulkanBuffer> &&buffer);

  /**
   * @brief Get buffer
   *
   * @param handle Buffer handle
   * @return Vulkan buffer
   */
  inline const std::unique_ptr<VulkanBuffer> &
  getBuffer(BufferHandle handle) const {
    return mBuffers.at(handle);
  }

  /**
   * @brief Get buffers
   *
   * @return List of buffers
   */
  inline const BufferMap &getBuffers() const { return mBuffers; }

  /**
   * @brief Add texture
   *
   * @param handle Texture handle
   * @param texture Vulkan texture
   */
  void addTexture(TextureHandle handle,
                  std::unique_ptr<VulkanTexture> &&texture);

  /**
   * @brief Remove texture
   *
   * @param handle Texture handle
   */
  void removeTexture(TextureHandle handle);

  /**
   * @brief Get textures
   *
   * @return List of textures
   */
  inline const TextureMap &getTextures() const { return mTextures; }

  /**
   * @brief Update texture
   *
   * @param handle Texture handle
   * @param texture Vulkan texture
   */
  void updateTexture(TextureHandle handle,
                     std::unique_ptr<VulkanTexture> &&texture);

  /**
   * @brief Add render pass
   *
   * @param handle Render pass handle
   * @param renderPass Vulkan render pass
   */
  void addRenderPass(rhi::RenderPassHandle handle,
                     std::unique_ptr<VulkanRenderPass> &&renderPass);

  /**
   * @brief Update render pass
   *
   * @param handle Render pass handle
   * @param renderPass Vulkan render pass
   */
  void updateRenderPass(rhi::RenderPassHandle handle,
                        std::unique_ptr<VulkanRenderPass> &&renderPass);

  /**
   * @brief Get render passes
   *
   * @return List of render passes
   */
  inline const RenderPassMap &getRenderPasses() const { return mRenderPasses; }

  /**
   * @brief Add framebuffer
   *
   * @param handle Framebuffer handle
   * @param framebuffer Vulkan framebuffer
   */
  void addFramebuffer(FramebufferHandle handle,
                      std::unique_ptr<VulkanFramebuffer> &&framebuffer);

  /**
   * @brief Update framebuffer
   *
   * @param handle Framebuffer handle
   * @param framebuffer Vulkan framebuffer
   */
  void updateFramebuffer(rhi::RenderPassHandle handle,
                         std::unique_ptr<VulkanFramebuffer> &&framebuffer);

  /**
   * @brief Get framebuffers
   *
   * @return List of framebuffers
   */
  inline const FramebufferMap &getFramebuffers() const { return mFramebuffers; }

  /**
   * @brief Add pipeline
   *
   * @param handle Pipeline handle
   * @param pipeline Vulkan pipeline
   */
  void addPipeline(PipelineHandle handle,
                   std::unique_ptr<VulkanPipeline> &&pipeline);

  /**
   * @brief Update pipeline
   *
   * @param handle Pipeline handle
   * @param pipeline Vulkan pipeline
   */
  void updatePipeline(rhi::RenderPassHandle handle,
                      std::unique_ptr<VulkanPipeline> &&pipeline);

  /**
   * @brief Get pipelines
   *
   * @return List of pipelines
   */
  inline const PipelineMap &getPipelines() const { return mPipelines; }

private:
  ShaderMap mShaders;
  BufferMap mBuffers;
  TextureMap mTextures;
  RenderPassMap mRenderPasses;
  FramebufferMap mFramebuffers;
  PipelineMap mPipelines;
};

} // namespace liquid::rhi

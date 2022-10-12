#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid::rhi {

class VulkanBuffer;
class VulkanTexture;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanPipeline;
class VulkanShader;

/**
 * @brief Vulkan resource registry
 *
 * Stores all the resources associated
 * with a device
 */
class VulkanResourceRegistry {

  /**
   * @brief Resource map for Vulkan resources
   *
   * @tparam THandle Handle
   * @tparam TResource Resource
   */
  template <class THandle, class TResource> struct ResourceMap {
    /**
     * @brief Map type
     */
    using Map = std::unordered_map<THandle, std::unique_ptr<TResource>>;

    /**
     * @brief Map of handles and resources
     */
    Map map;

    /**
     * @brief Last handle
     *
     * Used for auto generation
     */
    uint32_t lastHandle = 1;
  };

  using ShaderMap = ResourceMap<ShaderHandle, VulkanShader>;
  using BufferMap = ResourceMap<BufferHandle, VulkanBuffer>;
  using TextureMap = ResourceMap<TextureHandle, VulkanTexture>;
  using RenderPassMap = ResourceMap<RenderPassHandle, VulkanRenderPass>;
  using FramebufferMap = ResourceMap<FramebufferHandle, VulkanFramebuffer>;
  using PipelineMap = ResourceMap<PipelineHandle, VulkanPipeline>;

public:
  /**
   * @brief Set shader
   *
   * @param shader Vulkan shader
   * @return New shader handle
   */
  ShaderHandle setShader(std::unique_ptr<VulkanShader> &&shader);

  /**
   * @brief Delete shader
   *
   * @param handle Shader handle
   */
  void deleteShader(ShaderHandle handle);

  /**
   * @brief Get shaders
   *
   * @return List of shaders
   */
  inline const ShaderMap::Map &getShaders() const { return mShaders.map; }

  /**
   * @brief Set buffer
   *
   * @param buffer Vulkan buffer
   * @return New buffer handle
   */
  BufferHandle setBuffer(std::unique_ptr<VulkanBuffer> &&buffer);

  /**
   * @brief Delete buffer
   *
   * @param handle Buffer handle
   */
  void deleteBuffer(BufferHandle handle);

  /**
   * @brief Check if buffer exists
   *
   * @param handle Buffer handle
   * @retval true Buffer exists
   * @retval false Buffer does not exist
   */
  inline bool hasBuffer(BufferHandle handle) const {
    return mBuffers.map.find(handle) != mBuffers.map.end();
  }

  /**
   * @brief Get buffers
   *
   * @return List of buffers
   */
  inline const BufferMap::Map &getBuffers() const { return mBuffers.map; }

  /**
   * @brief Set texture
   *
   * @param texture Vulkan texture
   * @return New texture handle
   */
  TextureHandle setTexture(std::unique_ptr<VulkanTexture> &&texture);

  /**
   * @brief Recreate existing texture with new parameter
   *
   * @param handle Texture handle
   * @param texture Vulkan texture
   */
  void recreateTexture(TextureHandle handle,
                       std::unique_ptr<VulkanTexture> &&texture);

  /**
   * @brief Delete texture
   *
   * @param handle Texture handle
   */
  void deleteTexture(TextureHandle handle);

  /**
   * @brief Delete dangling swapchain relative textures
   *
   * If a texture does not exist or is not swapchain
   * relative, remove it from the swapchain relative
   * textures list
   */
  void deleteDanglingSwapchainRelativeTextures();

  /**
   * @brief Get swapchain relative textures
   *
   * @return Swapchain relative textures
   */
  inline const std::set<TextureHandle> &getSwapchainRelativeTextures() const {
    return mSwapchainRelativeTextures;
  }

  /**
   * @brief Get textures
   *
   * @return List of textures
   */
  inline const TextureMap::Map &getTextures() const { return mTextures.map; }

  /**
   * @brief Set render pass
   *
   * @param renderPass Vulkan render pass
   * @return New render pass handle
   */
  RenderPassHandle
  setRenderPass(std::unique_ptr<VulkanRenderPass> &&renderPass);

  /**
   * @brief Delete render pass
   *
   * @param handle Render pass handle
   */
  void deleteRenderPass(rhi::RenderPassHandle handle);

  /**
   * @brief Get render passes
   *
   * @return List of render passes
   */
  inline const RenderPassMap::Map &getRenderPasses() const {
    return mRenderPasses.map;
  }

  /**
   * @brief Set framebuffer
   *
   * @param framebuffer Vulkan framebuffer
   * @return New framebuffer handle
   */
  FramebufferHandle
  setFramebuffer(std::unique_ptr<VulkanFramebuffer> &&framebuffer);

  /**
   * @brief Delete framebuffer
   *
   * @param handle Framebuffer handle
   */
  void deleteFramebuffer(rhi::FramebufferHandle handle);

  /**
   * @brief Get framebuffers
   *
   * @return List of framebuffers
   */
  inline const FramebufferMap::Map &getFramebuffers() const {
    return mFramebuffers.map;
  }

  /**
   * @brief Set pipeline
   *
   * @param pipeline Vulkan pipeline
   * @return New pipeline handle
   */
  PipelineHandle setPipeline(std::unique_ptr<VulkanPipeline> &&pipeline);

  /**
   * @brief Delete pipeline
   *
   * @param handle Pipeline handle
   */
  void deletePipeline(rhi::PipelineHandle handle);

  /**
   * @brief Get pipelines
   *
   * @return List of pipelines
   */
  inline const PipelineMap::Map &getPipelines() const { return mPipelines.map; }

private:
  BufferMap mBuffers;
  TextureMap mTextures;
  ShaderMap mShaders;
  RenderPassMap mRenderPasses;
  FramebufferMap mFramebuffers;
  PipelineMap mPipelines;

  std::set<TextureHandle> mSwapchainRelativeTextures;
};

} // namespace liquid::rhi

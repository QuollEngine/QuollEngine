#pragma once

#include "RenderHandle.h"
#include "BufferDescription.h"
#include "TextureDescription.h"
#include "RenderPassDescription.h"
#include "FramebufferDescription.h"
#include "PipelineDescription.h"
#include "ShaderDescription.h"

namespace liquid::experimental {

static constexpr uint32_t RESERVED_HANDLE_SIZE = 20;

/**
 * @brief Registry map
 *
 * Stores descriptions in a hash map
 * and provides information about newly
 * added, updated, and deleted descriptions
 *
 * @tparam THandle Resource Handle type
 * @tparam TDescription Description type
 * @tparam TStartingHandle Starting handle id
 */
template <class THandle, class TDescription,
          THandle TStartingHandleId =
              static_cast<THandle>(RESERVED_HANDLE_SIZE)>
class ResourceRegistryMap {
  using HandleList = std::vector<THandle>;
  static_assert(TStartingHandleId >= 1, "Starting handle cannot be ZERO");

public:
  /**
   * @brief Add description
   *
   * @param description Description
   * @return Resource handle
   */
  THandle addDescription(const TDescription &description) {
    THandle newHandle = mLastHandle++;
    mDescriptions.insert({newHandle, description});
    mDirtyCreates.push_back(newHandle);

    return newHandle;
  }

  /**
   * @brief Update description
   *
   * @param handle Resource handle
   * @param description Description
   */
  inline void updateDescription(THandle handle,
                                const TDescription &description) {
    mDescriptions.at(handle) = description;
    mDirtyUpdates.push_back(handle);
  }

  /**
   * @brief Delete description
   *
   * @param handle Resource handle
   */
  inline void deleteDescription(THandle handle) {
    mDescriptions.erase(handle);
    mDirtyDeletes.push_back(handle);
  }

  /**
   * @brief Get description
   *
   * @param handle Resource handle
   * @return Description
   */
  inline const TDescription &getDescription(THandle handle) const {
    return mDescriptions.at(handle);
  }

  /**
   * @brief Check if resource description exists
   *
   * @param handle Resource handle
   * @retval true Description exists
   * @retval false Description does not exist
   */
  inline bool hasDescription(THandle handle) const {
    return mDescriptions.find(handle) != mDescriptions.end();
  }

  /**
   * @brief Get newly added resources
   *
   * @return List of newly added resources
   */
  inline const HandleList &getDirtyCreates() const { return mDirtyCreates; }

  /**
   * @brief Get newly updated resources
   *
   * @return List of newly updated resources
   */
  inline const HandleList &getDirtyUpdates() const { return mDirtyUpdates; }

  /**
   * @brief Get newly deleted resources
   *
   * @return List of newly deleted resources
   */
  inline const HandleList &getDirtyDeletes() const { return mDirtyDeletes; }

  /**
   * @brief Clear dirty creates
   */
  inline void clearDirtyCreates() { mDirtyCreates.clear(); }

  /**
   * @brief Clear dirty deletes
   */
  inline void clearDirtyDeletes() { mDirtyDeletes.clear(); }

  /**
   * @brief Clear dirty updates
   */
  inline void clearDirtyUpdates() { mDirtyUpdates.clear(); }

private:
  std::unordered_map<THandle, TDescription> mDescriptions;
  std::vector<THandle> mDirtyCreates;
  std::vector<THandle> mDirtyUpdates;
  std::vector<THandle> mDirtyDeletes;

  // ZERO means undefined
  THandle mLastHandle = TStartingHandleId;
};

class ResourceRegistry {
public:
  /**
   * @brief Add shader
   *
   * @param description Shader description
   * @return Shader handle
   */
  ShaderHandle addShader(const ShaderDescription &description);

  /**
   * @brief Delete shader
   *
   * @param handle Shader handle
   */
  void deleteShader(ShaderHandle handle);

  /**
   * @brief Get shader map
   *
   * @return Shader map
   */
  inline ResourceRegistryMap<ShaderHandle, ShaderDescription> &getShaderMap() {
    return mShaders;
  }

  /**
   * @brief Add buffer
   *
   * @param description Buffer description
   * @return Buffer handle
   */
  BufferHandle addBuffer(const BufferDescription &description);

  /**
   * @brief Delete buffer
   *
   * @param handle Buffer handle
   */
  void deleteBuffer(BufferHandle handle);

  /**
   * @brief Update buffer
   *
   * @param handle Buffer handle
   * @param description Buffer description
   */
  void updateBuffer(BufferHandle handle, const BufferDescription &description);

  /**
   * @brief Get buffer map
   *
   * @return Buffer map
   */
  inline ResourceRegistryMap<BufferHandle, BufferDescription> &getBufferMap() {
    return mBuffers;
  }

  /**
   * @brief Add texture
   *
   * @param description Texture description
   * @return Texture handle
   */
  TextureHandle addTexture(const TextureDescription &description);

  /**
   * @brief Remove texture
   *
   * @param handle Texture handle
   */
  void deleteTexture(TextureHandle handle);

  /**
   * @brief Get texture map
   *
   * @return Texture map
   */
  inline ResourceRegistryMap<TextureHandle, TextureDescription> &
  getTextureMap() {
    return mTextures;
  }

  /**
   * @brief Add render pass
   *
   * @param description Render pass description
   * @return RenderPass handle
   */
  RenderPassHandle addRenderPass(const RenderPassDescription &description);

  /**
   * @brief Update render pass
   *
   * @param handle Render pass handle
   * @param description Render pass Description
   */
  void updateRenderPass(RenderPassHandle handle,
                        const RenderPassDescription &description);

  /**
   * @brief Remove render pass
   *
   * @param handle Render pass handle
   */
  void deleteRenderPass(RenderPassHandle handle);

  /**
   * @brief Get render pass map
   *
   * @return Render pass map
   */
  inline ResourceRegistryMap<RenderPassHandle, RenderPassDescription> &
  getRenderPassMap() {
    return mRenderPasses;
  }

  /**
   * @brief Add framebuffer
   *
   * @param description Render pass description
   * @return Framebuffer handle
   */
  FramebufferHandle addFramebuffer(const FramebufferDescription &description);

  /**
   * @brief Update framebuffer
   *
   * @param handle Framebuffer handle
   * @param description Framebuffer Description
   */
  void updateFramebuffer(FramebufferHandle handle,
                         const FramebufferDescription &description);

  /**
   * @brief Remove framebuffer
   *
   * @param handle Render pass handle
   */
  void deleteFramebuffer(FramebufferHandle handle);

  /**
   * @brief Get framebuffer map
   *
   * @return Render pass map
   */
  inline ResourceRegistryMap<FramebufferHandle, FramebufferDescription> &
  getFramebufferMap() {
    return mFramebuffers;
  }

  /**
   * @brief Add pipeline
   *
   * @param description Render pass description
   * @return Pipeline handle
   */
  PipelineHandle addPipeline(const PipelineDescription &description);

  /**
   * @brief Update pipeline
   *
   * @param handle Pipeline handle
   * @param description Pipeline Description
   */
  void updatePipeline(PipelineHandle handle,
                      const PipelineDescription &description);

  /**
   * @brief Remove pipeline
   *
   * @param handle Render pass handle
   */
  void deletePipeline(PipelineHandle handle);

  /**
   * @brief Get pipeline map
   *
   * @return Render pass map
   */
  inline ResourceRegistryMap<PipelineHandle, PipelineDescription> &
  getPipelineMap() {
    return mPipelines;
  }

private:
  ResourceRegistryMap<ShaderHandle, ShaderDescription> mShaders;
  ResourceRegistryMap<BufferHandle, BufferDescription> mBuffers;
  ResourceRegistryMap<TextureHandle, TextureDescription> mTextures;
  ResourceRegistryMap<RenderPassHandle, RenderPassDescription> mRenderPasses;
  ResourceRegistryMap<FramebufferHandle, FramebufferDescription> mFramebuffers;
  ResourceRegistryMap<PipelineHandle, PipelineDescription> mPipelines;
};

} // namespace liquid::experimental

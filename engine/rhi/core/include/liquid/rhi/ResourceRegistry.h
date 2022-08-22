#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/BufferDescription.h"
#include "liquid/rhi/TextureDescription.h"
#include "liquid/rhi/RenderPassDescription.h"
#include "liquid/rhi/FramebufferDescription.h"
#include "liquid/rhi/PipelineDescription.h"
#include "liquid/rhi/ShaderDescription.h"

namespace liquid::rhi {

static constexpr uint32_t RESERVED_HANDLE_SIZE = 20;

enum class ResourceRegistryState { Set, Delete };

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
          THandle TStartingHandleId = THandle(RESERVED_HANDLE_SIZE)>
class ResourceRegistryMap {
  using HandleList = std::vector<THandle>;
  static_assert(TStartingHandleId != THandle::Invalid,
                "Starting handle cannot be invalid");

public:
  /**
   * @brief Set description
   *
   * Creates new resource or updates
   * existing one
   *
   * @param description Description
   * @param handle Resource handle
   * @return New or existing resource handle
   */
  inline THandle setDescription(const TDescription &description,
                                THandle handle) {
    LIQUID_ASSERT(handle == THandle::Invalid ||
                      mDescriptions.find(handle) != mDescriptions.end(),
                  "Cannot update non existent resource");

    auto retHandle = handle == THandle::Invalid ? mLastHandle : handle;
    mDescriptions.insert_or_assign(retHandle, description);
    mStagedResources.insert_or_assign(retHandle, ResourceRegistryState::Set);
    mLastHandle = static_cast<THandle>(rhi::castHandleToUint(mLastHandle) +
                                       (handle == THandle::Invalid));
    return retHandle;
  }

  /**
   * @brief Delete description
   *
   * @param handle Resource handle
   */
  inline void deleteDescription(THandle handle) {
    mDescriptions.erase(handle);
    mStagedResources.insert_or_assign(handle, ResourceRegistryState::Delete);
  }

  /**
   * @brief Clear staged resources
   */
  inline void clearStagedResources() { mStagedResources.clear(); }

  /**
   * @brief Get staged resources
   *
   * @return Staged resources
   */
  inline const std::map<THandle, ResourceRegistryState> &
  getStagedResources() const {
    return mStagedResources;
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
   * @brief Get all descriptions
   *
   * @return List of descriptions
   */
  const std::unordered_map<THandle, TDescription> &getDescriptions() const {
    return mDescriptions;
  }

private:
  std::unordered_map<THandle, TDescription> mDescriptions;
  std::map<THandle, ResourceRegistryState> mStagedResources;

  // ZERO means undefined
  THandle mLastHandle = TStartingHandleId;
};

/**
 * @brief Resource registry
 *
 * Stores descriptions of all the resources
 */
class ResourceRegistry {
public:
  /**
   * @brief Set render pass
   *
   * @param description Render pass description
   * @param handle Render pass handle
   * @return Render pass handle
   */
  RenderPassHandle
  setRenderPass(const RenderPassDescription &description,
                RenderPassHandle handle = RenderPassHandle::Invalid);

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
   * @brief Set framebuffer
   *
   * @param description Render pass description
   * @param handle Framebuffer handle
   * @return Framebuffer handle
   */
  FramebufferHandle
  setFramebuffer(const FramebufferDescription &description,
                 FramebufferHandle handle = FramebufferHandle::Invalid);

  /**
   * @brief Remove framebuffer
   *
   * @param handle Render pass handle
   */
  void deleteFramebuffer(FramebufferHandle handle);

  /**
   * @brief Get framebuffer map
   *
   * @return Framebuffer map
   */
  inline ResourceRegistryMap<FramebufferHandle, FramebufferDescription> &
  getFramebufferMap() {
    return mFramebuffers;
  }

  /**
   * @brief Set pipeline
   *
   * @param description Render pass description
   * @param handle Pipeline handle
   * @return Pipeline handle
   */
  PipelineHandle setPipeline(const PipelineDescription &description,
                             PipelineHandle handle = PipelineHandle::Invalid);

  /**
   * @brief Remove pipeline
   *
   * @param handle Render pass handle
   */
  void deletePipeline(PipelineHandle handle);

  /**
   * @brief Get pipeline map
   *
   * @return Pipeline map
   */
  inline ResourceRegistryMap<PipelineHandle, PipelineDescription> &
  getPipelineMap() {
    return mPipelines;
  }

private:
  ResourceRegistryMap<rhi::RenderPassHandle, RenderPassDescription>
      mRenderPasses;
  ResourceRegistryMap<FramebufferHandle, FramebufferDescription> mFramebuffers;
  ResourceRegistryMap<PipelineHandle, PipelineDescription> mPipelines;
};

} // namespace liquid::rhi

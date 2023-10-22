#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "RenderStorage.h"

namespace quoll {

/**
 * @brief Render graph texture view description
 */
struct RGTextureViewDescription {
  /**
   * Texture index in render graph registry
   */
  usize textureIndex = 0;

  /**
   * Base mip level
   */
  u32 baseMipLevel = 0;

  /**
   * Mip level count
   */
  u32 mipLevelCount = 1;

  /**
   * Base layer
   */
  u32 baseLayer = 0;

  /**
   * Layer count
   */
  u32 layerCount = 1;
};

enum class RGResourceState { Transient, Real };

/**
 * @brief Render graph resource cache
 *
 * @tparam THandle Handle type
 * @tparam TDescription Description type
 */
template <class THandle, class TDescription> class RenderGraphResourceCache {
  using ResourceReadyFn = std::function<void(THandle, RenderStorage &)>;

public:
  /**
   * @brief Allocate space for new resource
   *
   * @param description Description
   * @return Resource index
   */
  usize allocate(TDescription description) {
    auto index = mRealResources.size();
    mRealResources.push_back(THandle{0});
    mDescriptions.push_back(description);
    mResourceReadyFns.push_back([](auto, auto &) {});
    mResourceStates.push_back(RGResourceState::Transient);

    return index;
  }

  /**
   * @brief Allocate space for existing resource
   *
   * @param handle Resource handle
   * @return Resource index
   */
  usize allocate(THandle handle) {
    auto index = mRealResources.size();
    mRealResources.push_back(handle);
    mDescriptions.push_back({});
    mResourceReadyFns.push_back([](auto, auto &) {});
    mResourceStates.push_back(RGResourceState::Real);

    return index;
  }

  /**
   * @brief Get state of render graph resource
   *
   * @param index Resource index
   * @return Render graph resource state
   */
  RGResourceState getResourceState(usize index) const {
    return mResourceStates.at(index);
  }

  /**
   * @brief Get real resource
   *
   * @param index Resource indexResourceType
   * @return Resource handleindex
   */
  THandle get(usize index) { return mRealResources.at(index); }

  /**
   * @brief Get real resources
   *
   * @return Real resources
   */
  inline const std::vector<THandle> &getRealResources() const {
    return mRealResources;
  }

  /**
   * @brief Get description
   *
   * @param index Resource index
   * @return Resource index
   */
  inline const TDescription &getDescription(usize index) const {
    return mDescriptions.at(index);
  }

  /**
   * @brief Set handle
   *
   * @tparam THandle Handle type
   * @param index Resource index
   * @param handle Handle
   */
  void set(usize index, THandle handle) { mRealResources.at(index) = handle; }

  /**
   * @brief Set on resource ready function
   *
   * @param index Resource index
   * @param resourceReadyFn Resource ready function
   */
  template <class TFunction>
  void setResourceReady(usize index, TFunction &&resourceReadyFn) {
    mResourceReadyFns.at(index) = resourceReadyFn;
  }

  /**
   * @brief Call resource ready
   *
   * @param index Resource index
   * @param storage Render storage
   */
  void callResourceReady(usize index, RenderStorage &storage) {
    mResourceReadyFns.at(index)(mRealResources.at(index), storage);
  }

private:
  std::vector<THandle> mRealResources;
  std::vector<ResourceReadyFn> mResourceReadyFns;
  std::vector<TDescription> mDescriptions;
  std::vector<RGResourceState> mResourceStates;
};

/**
 * @brief Render graph registry
 */
class RenderGraphRegistry {
public:
  /**
   * Resource ready function
   *
   * @tparam THandle Handle type
   */
  template <class THandle>
  using ResourceReadyFn = std::function<void(THandle, RenderStorage &)>;

public:
  /**
   * @brief Render graph registry
   *
   * @tparam THandle Handle type
   * @param index Index
   * @return Resource handle
   */
  template <class THandle> inline THandle get(usize index) {
    return getCache<THandle>().get(index);
  }

  /**
   * @brief Render graph registry
   *
   * @tparam THandle Handle type
   * @param index Index
   * @return Resource handle
   */
  template <class THandle> inline auto getDescription(usize index) {
    return getCache<THandle>().getDescription(index);
  }

  /**
   * @brief Get all real resources
   *
   * @return Real resources
   */
  template <class THandle>
  inline const std::vector<THandle> &getRealResources() const {
    return getCache<THandle>().getRealResources();
  }

  /**
   * @brief Allocate space for resource
   *
   * @tparam THandle Handle type
   * @param handle Handle
   * @return Newly created resource index
   */
  template <class THandle> usize allocate(THandle handle = THandle::Null) {
    return getCache<THandle>().allocate(handle);
  }

  /**
   * @brief Allocate space for resource
   *
   * @tparam THandle Handle type
   * @tparam TDescription Description type
   * @param description Description
   * @return Newly created resource index
   */
  template <class THandle, class TDescription>
  usize allocate(TDescription description) {
    return getCache<THandle>().allocate(description);
  }

  /**
   * @brief Get render graph resource type
   *
   * @tparam THandle Handle type
   * @param index Resource index
   * @return Render graph resource type
   */
  template <class THandle>
  inline RGResourceState getResourceState(usize index) const {
    return getCache<THandle>().getResourceState(index);
  }

  /**
   * @brief Set handle
   *
   * @tparam THandle Handle type
   * @param index Resource index
   * @param handle Handle
   */
  template <class THandle> void set(usize index, THandle handle) {
    getCache<THandle>().set(index, handle);
  }

  /**
   * @brief Set on resource ready function
   *
   * @param index Resource index
   * @param resourceReadyFn Resource ready function
   */
  template <class THandle, class TFunction>
  inline void setResourceReady(usize index, TFunction &&resourceReadyFn) {
    getCache<THandle>().setResourceReady(index, resourceReadyFn);
  }

  /**
   * @brief Call resource ready
   *
   * @param index Resource index
   * @param storage Render storage
   */
  template <class THandle>
  inline void callResourceReady(usize index, RenderStorage &storage) {
    getCache<THandle>().callResourceReady(index, storage);
  }

private:
  template <class THandle> constexpr auto &getCache() {
    if constexpr (std::is_same_v<THandle, rhi::TextureHandle>) {
      return mTextureCache;
    }
  }

  template <class THandle> constexpr const auto &getCache() const {
    if constexpr (std::is_same_v<THandle, rhi::TextureHandle>) {
      return mTextureCache;
    }
  }

private:
  using TextureDesc = std::variant<std::monostate, rhi::TextureDescription,
                                   RGTextureViewDescription>;

  RenderGraphResourceCache<rhi::TextureHandle, TextureDesc> mTextureCache;
};

} // namespace quoll

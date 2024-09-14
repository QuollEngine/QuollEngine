#pragma once

#include "quoll/rhi/BufferDescription.h"
#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/TextureDescription.h"

namespace quoll {

class RenderStorage;

struct RGTextureViewDescription {
  usize textureIndex = 0;

  u32 baseMipLevel = 0;

  u32 mipLevelCount = 1;

  u32 baseLayer = 0;

  u32 layerCount = 1;
};

enum class RGResourceState { Transient, Real };

template <class THandle, class TDescription> class RenderGraphResourceCache {
  using ResourceReadyFn = std::function<void(THandle, RenderStorage &)>;

public:
  usize allocate(TDescription description) {
    auto index = mRealResources.size();
    mRealResources.push_back(THandle{0});
    mDescriptions.push_back(description);
    mResourceReadyFns.push_back([](auto, auto &) {});
    mResourceStates.push_back(RGResourceState::Transient);

    return index;
  }

  usize allocate(THandle handle) {
    auto index = mRealResources.size();
    mRealResources.push_back(handle);
    mDescriptions.push_back({});
    mResourceReadyFns.push_back([](auto, auto &) {});
    mResourceStates.push_back(RGResourceState::Real);

    return index;
  }

  RGResourceState getResourceState(usize index) const {
    return mResourceStates.at(index);
  }

  THandle get(usize index) { return mRealResources.at(index); }

  inline const std::vector<THandle> &getRealResources() const {
    return mRealResources;
  }

  inline const TDescription &getDescription(usize index) const {
    return mDescriptions.at(index);
  }

  void set(usize index, THandle handle) { mRealResources.at(index) = handle; }

  template <class TFunction>
  void setResourceReady(usize index, TFunction &&resourceReadyFn) {
    mResourceReadyFns.at(index) = resourceReadyFn;
  }

  void callResourceReady(usize index, RenderStorage &storage) {
    mResourceReadyFns.at(index)(mRealResources.at(index), storage);
  }

private:
  std::vector<THandle> mRealResources;
  std::vector<ResourceReadyFn> mResourceReadyFns;
  std::vector<TDescription> mDescriptions;
  std::vector<RGResourceState> mResourceStates;
};

class RenderGraphRegistry {
public:
  template <class THandle>
  using ResourceReadyFn = std::function<void(THandle, RenderStorage &)>;

public:
  template <class THandle> inline THandle get(usize index) {
    return getCache<THandle>().get(index);
  }

  template <class THandle> inline auto getDescription(usize index) {
    return getCache<THandle>().getDescription(index);
  }

  template <class THandle>
  inline const std::vector<THandle> &getRealResources() const {
    return getCache<THandle>().getRealResources();
  }

  template <class THandle> usize allocate(THandle handle = THandle::Null) {
    return getCache<THandle>().allocate(handle);
  }

  template <class THandle, class TDescription>
  usize allocate(TDescription description) {
    return getCache<THandle>().allocate(description);
  }

  template <class THandle>
  inline RGResourceState getResourceState(usize index) const {
    return getCache<THandle>().getResourceState(index);
  }

  template <class THandle> void set(usize index, THandle handle) {
    getCache<THandle>().set(index, handle);
  }

  template <class THandle, class TFunction>
  inline void setResourceReady(usize index, TFunction &&resourceReadyFn) {
    getCache<THandle>().setResourceReady(index, resourceReadyFn);
  }

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

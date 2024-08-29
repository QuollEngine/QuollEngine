#pragma once

#include "AssetData.h"
#include "AssetHandle.h"
#include "AssetRegistry.h"

namespace quoll {

template <class TAssetData> class AssetRef {
public:
  constexpr AssetRef(AssetRegistry *registry, AssetHandle<TAssetData> handle)
      : mRegistry(registry), mHandle(handle) {}

  constexpr operator bool() const { return mHandle; }
  constexpr TAssetData *operator->() { return &mRegistry->get(mHandle); }
  constexpr TAssetData &get() { return mRegistry->get(mHandle); }
  constexpr const TAssetData &get() const { return mRegistry->get(mHandle); }
  constexpr AssetData<TAssetData> &meta() {
    return mRegistry->getMeta(mHandle);
  }
  constexpr const AssetHandle<TAssetData> &handle() const { return mHandle; }

private:
  AssetHandle<TAssetData> mHandle;
  AssetRegistry *mRegistry;
};

} // namespace quoll

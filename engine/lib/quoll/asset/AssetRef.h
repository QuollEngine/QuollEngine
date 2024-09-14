#pragma once

#include "AssetData.h"
#include "AssetHandle.h"
#include "AssetMap.h"

namespace quoll {

template <class TAssetData> class AssetRef {
public:
  constexpr AssetRef() = default;
  constexpr explicit AssetRef(AssetMap<TAssetData> &map,
                              AssetHandle<TAssetData> handle)
      : mMap(&map), mHandle(handle) {
    mMap->take(mHandle);
  }

  constexpr AssetRef(const AssetRef &rhs)
      : mHandle(rhs.mHandle), mMap(rhs.mMap) {
    if (mMap) {
      mMap->take(mHandle);
    }
  }

  constexpr auto &operator=(const AssetRef &rhs) {
    if (mMap) {
      mMap->release(mHandle);
    }

    mHandle = rhs.mHandle;
    mMap = rhs.mMap;

    if (mMap) {
      mMap->take(mHandle);
    }
    return *this;
  }

  constexpr AssetRef(AssetRef &&rhs) : mHandle(rhs.mHandle), mMap(rhs.mMap) {
    rhs.mHandle = AssetHandle<TAssetData>();
    rhs.mMap = nullptr;
  }

  constexpr auto &operator=(AssetRef &&rhs) {
    if (mMap) {
      mMap->release(mHandle);
    }

    mHandle = rhs.mHandle;
    mMap = rhs.mMap;

    rhs.mHandle = AssetHandle<TAssetData>();
    rhs.mMap = nullptr;

    return *this;
  }

  constexpr ~AssetRef() noexcept {
    if (mMap) {
      mMap->release(mHandle);
      mMap = nullptr;
    }
  }

  constexpr operator bool() const { return mHandle && mMap->hasData(mHandle); }
  constexpr const TAssetData *operator->() const { return &get(); }

  constexpr const TAssetData &get() const {
    QuollAssert(mHandle, "Handle is null");
    return mMap->get(mHandle);
  }

  constexpr const AssetMeta &meta() const {
    QuollAssert(mHandle, "Handle is null");
    return mMap->getMeta(mHandle);
  }

  constexpr const AssetHandle<TAssetData> &handle() const { return mHandle; }

  constexpr bool valid() const { return mHandle; }

private:
  AssetHandle<TAssetData> mHandle;
  AssetMap<TAssetData> *mMap = nullptr;
};

} // namespace quoll

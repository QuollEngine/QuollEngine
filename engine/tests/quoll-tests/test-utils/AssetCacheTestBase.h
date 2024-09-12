#pragma once

#include "quoll/asset/AssetCache.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheUtils.h"

class AssetCacheTestBase : public ::testing::Test {
public:
  static const quoll::Path CachePath;

public:
  AssetCacheTestBase();

  template <typename TAssetData>
  quoll::AssetRef<TAssetData> createAsset(TAssetData data = {}) {
    return createAssetInCache(cache, data);
  }

  template <typename TAssetData>
  quoll::Result<quoll::AssetRef<TAssetData>> requestAndWait(quoll::Uuid uuid) {
    auto res = cache.request<TAssetData>(uuid);
    if (!res) {
      return res;
    }

    const auto &futures = cache.waitForIdle();

    auto it = futures.find(uuid);
    if (it == futures.end()) {
      return res;
    }

    auto loadRes = it->second;
    if (!loadRes) {
      return loadRes.error();
    }

    return {res.data(), loadRes.warnings()};
  }

  void SetUp() override;

  void TearDown() override;

public:
  quoll::AssetCache cache;
};

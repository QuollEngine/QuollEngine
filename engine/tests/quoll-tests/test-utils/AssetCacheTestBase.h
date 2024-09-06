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

  void SetUp() override;

  void TearDown() override;

public:
  quoll::AssetCache cache;
};

#pragma once

#include "quoll/asset/AssetCache.h"
#include "quoll-tests/Testing.h"

class AssetCacheTestBase : public ::testing::Test {
public:
  static const quoll::Path CachePath;

public:
  AssetCacheTestBase();

  template <typename TAssetData>
  quoll::AssetRef<TAssetData> createAsset(TAssetData data = {}) {
    quoll::AssetData<TAssetData> info{};
    info.type = quoll::AssetCache::getAssetType<TAssetData>();
    info.uuid = quoll::Uuid::generate();
    info.data = data;

    cache.getRegistry().add(info);

    return cache.request<TAssetData>(info.uuid).data();
  }

  void SetUp() override;

  void TearDown() override;

public:
  quoll::AssetCache cache;
};

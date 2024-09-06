#pragma once

#include "quoll/asset/AssetCache.h"

template <typename TAssetData>
quoll::AssetRef<TAssetData> createAssetInCache(quoll::AssetCache &cache,
                                               TAssetData data = {},
                                               quoll::String name = "") {
  quoll::AssetMeta meta{};
  meta.type = quoll::AssetCache::getAssetType<TAssetData>();
  meta.uuid = quoll::Uuid::generate();
  meta.name = name;

  auto handle = cache.getRegistry().allocate<TAssetData>(meta);
  cache.getRegistry().store(handle, data);

  return cache.request<TAssetData>(meta.uuid).data();
}

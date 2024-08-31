#pragma once

#include "quoll/editor/actions/Action.h"
#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor-tests/Testing.h"

class ActionTestBase : public ::testing::Test {
public:
  ActionTestBase();

  template <typename TAssetData>
  quoll::AssetRef<TAssetData> createAsset(TAssetData data = {}) {
    quoll::AssetData<TAssetData> info{};
    info.type = quoll::AssetCache::getAssetType<TAssetData>();
    info.uuid = quoll::Uuid::generate();
    info.data = data;

    assetCache.getRegistry().add(info);

    return assetCache.request<TAssetData>(info.uuid).data();
  }

  quoll::AssetCache assetCache;
  quoll::editor::WorkspaceState state{};
};

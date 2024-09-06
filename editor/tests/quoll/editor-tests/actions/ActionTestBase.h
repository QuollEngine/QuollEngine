#pragma once

#include "quoll/editor/actions/Action.h"
#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheUtils.h"

class ActionTestBase : public ::testing::Test {
public:
  ActionTestBase();

  template <typename TAssetData>
  quoll::AssetRef<TAssetData> createAsset(TAssetData data = {}) {
    return createAssetInCache(assetCache, data);
  }

  quoll::AssetCache assetCache;
  quoll::editor::WorkspaceState state{};
};

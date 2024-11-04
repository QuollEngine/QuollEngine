#pragma once

#include "quoll/profiler/DebugPanel.h"

namespace quoll {

class AssetCache;

}

namespace quoll::debug {

class AssetsDebugPanel : public DebugPanel {
public:
  constexpr AssetsDebugPanel(AssetCache *assetCache)
      : mAssetCache(assetCache) {}

  void onRenderMenu() override;

  void onRender() override;

private:
  AssetCache *mAssetCache;
  bool mOpen = false;
};

} // namespace quoll::debug

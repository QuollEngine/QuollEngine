#pragma once

#include "quoll/editor/asset/AssetManager.h"
#include "Action.h"

namespace quoll::editor {

class ExportAsGame : public Action {
public:
  ExportAsGame(AssetManager &assetManager);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  AssetManager &mAssetManager;
};

} // namespace quoll::editor

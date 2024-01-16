#pragma once

#include "quoll/editor/asset/AssetManager.h"
#include "Action.h"

namespace quoll::editor {

class ExportAsGame : public Action {
public:
  ExportAsGame(AssetManager &assetManager);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  AssetManager &mAssetManager;
};

} // namespace quoll::editor

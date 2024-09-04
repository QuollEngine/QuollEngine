#include "quoll/core/Base.h"
#include "quoll/platform/tools/FileDialog.h"
#include "quoll/editor/core/GameExporter.h"
#include "ProjectActions.h"

namespace quoll::editor {

ExportAsGame::ExportAsGame(AssetManager &assetManager)
    : mAssetManager(assetManager) {}

ActionExecutorResult ExportAsGame::onExecute(WorkspaceState &state,
                                             AssetCache &assetCache) {
  auto path = platform::FileDialog::getFilePathFromCreateDialog({});

  mAssetManager.syncAssets();

  GameExporter exporter;
  exporter.exportGame(state.project, path);

  return ActionExecutorResult{};
}

bool ExportAsGame::predicate(WorkspaceState &state, AssetCache &assetCache) {
  return true;
}

} // namespace quoll::editor

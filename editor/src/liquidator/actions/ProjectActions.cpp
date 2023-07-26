#include "liquid/core/Base.h"
#include "liquid/platform/tools/FileDialog.h"
#include "liquidator/core/GameExporter.h"

#include "ProjectActions.h"

namespace liquid::editor {

ActionExecutorResult ExportAsGame::onExecute(WorkspaceState &state,
                                             AssetRegistry &assetRegistry) {
  auto path = platform::FileDialog::getFilePathFromCreateDialog({});

  GameExporter exporter;
  exporter.exportGame(state.project, path);

  return ActionExecutorResult{};
}

bool ExportAsGame::predicate(WorkspaceState &state,
                             AssetRegistry &assetRegistry) {
  return true;
}

} // namespace liquid::editor

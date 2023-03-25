#include "liquid/core/Base.h"
#include "liquid/platform-tools/NativeFileDialog.h"
#include "liquidator/core/GameExporter.h"

#include "ProjectActions.h"

namespace liquid::editor {

ActionExecutorResult ExportAsGame::onExecute(WorkspaceState &state) {
  platform_tools::NativeFileDialog nativeFileDialog;
  auto path = nativeFileDialog.getFilePathFromCreateDialog({});

  GameExporter exporter;
  exporter.exportGame(state.project, path);

  return ActionExecutorResult{};
}

bool ExportAsGame::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor

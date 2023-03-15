#include "liquid/core/Base.h"
#include "liquid/platform-tools/NativeFileDialog.h"
#include "liquidator/core/GameExporter.h"

#include "ExportAsGameAction.h"

namespace liquid::editor {

ActionExecutorResult ExportAsGameAction::onExecute(WorkspaceState &state) {
  platform_tools::NativeFileDialog nativeFileDialog;
  auto path = nativeFileDialog.getFilePathFromCreateDialog({});

  GameExporter exporter;
  exporter.exportGame(state.project, path);

  return ActionExecutorResult{};
}

bool ExportAsGameAction::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor

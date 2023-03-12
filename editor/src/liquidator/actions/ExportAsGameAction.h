#pragma once

#include "liquidator/actions/Action.h"
#include "liquid/platform-tools/NativeFileDialog.h"
#include "liquidator/core/GameExporter.h"

namespace liquid::editor {

static Action ExportAsGameAction{
    "ExportAsGame", [](WorkspaceState &state, std::any data) {
      platform_tools::NativeFileDialog nativeFileDialog;
      auto path = nativeFileDialog.getFilePathFromCreateDialog({});

      GameExporter exporter;
      exporter.exportGame(state.project, path);

      return ActionExecutorResult{};
    }};

} // namespace liquid::editor
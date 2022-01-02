#include "EditorGridPanel.h"

#include <imgui.h>

namespace liquidator {

void EditorGridPanel::render(SceneManager &sceneManager) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Editor")) {
      ImGui::MenuItem("Grid", nullptr, &open);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (!open) {
    return;
  }

  if (ImGui::Begin("Editor Grid", &open)) {
    bool showGridLines = sceneManager.getEditorGrid().gridLinesShown();
    if (ImGui::Checkbox("Show grid lines", &showGridLines)) {
      sceneManager.getEditorGrid().setGridLinesFlag(showGridLines);
    }

    bool showAxisLines = sceneManager.getEditorGrid().axisLinesShown();
    if (ImGui::Checkbox("Show axis lines", &showAxisLines)) {
      sceneManager.getEditorGrid().setAxisLinesFlag(showAxisLines);
    }
  }

  ImGui::End();
}

} // namespace liquidator
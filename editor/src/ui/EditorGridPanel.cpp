#include "liquid/core/Base.h"
#include "EditorGridPanel.h"

#include <imgui.h>

namespace liquidator {

void EditorGridPanel::render(SceneManager &sceneManager) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Editor")) {
      ImGui::MenuItem("Grid", nullptr, &mOpen);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (!mOpen) {
    return;
  }

  if (ImGui::Begin("Editor Grid", &mOpen, ImGuiWindowFlags_NoDocking)) {
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

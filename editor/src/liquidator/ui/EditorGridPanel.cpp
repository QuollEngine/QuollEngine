#include "liquid/core/Base.h"
#include "EditorGridPanel.h"

#include "liquid/imgui/Imgui.h"

namespace liquidator {

void EditorGridPanel::render(EditorManager &editorManager) {
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
    bool showGridLines = editorManager.getEditorGrid().gridLinesShown();
    if (ImGui::Checkbox("Show grid lines", &showGridLines)) {
      editorManager.getEditorGrid().setGridLinesFlag(showGridLines);
    }

    bool showAxisLines = editorManager.getEditorGrid().axisLinesShown();
    if (ImGui::Checkbox("Show axis lines", &showAxisLines)) {
      editorManager.getEditorGrid().setAxisLinesFlag(showAxisLines);
    }
  }

  ImGui::End();
}

} // namespace liquidator

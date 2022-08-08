#include "liquid/core/Base.h"
#include "EditorGridPanel.h"

#include "liquid/imgui/Imgui.h"
#include "Widgets.h"

namespace liquidator {

void EditorGridPanel::render(EditorManager &editorManager) {
  if (auto _ = widgets::MainMenuBar()) {
    if (ImGui::BeginMenu("Editor")) {
      ImGui::MenuItem("Grid", nullptr, &mOpen);
      ImGui::EndMenu();
    }
  }

  if (!mOpen) {
    return;
  }

  if (auto _ = widgets::FixedWindow("Editor Grid", mOpen)) {
    bool showGridLines = editorManager.getEditorGrid().gridLinesShown();
    if (ImGui::Checkbox("Show grid lines", &showGridLines)) {
      editorManager.getEditorGrid().setGridLinesFlag(showGridLines);
    }

    bool showAxisLines = editorManager.getEditorGrid().axisLinesShown();
    if (ImGui::Checkbox("Show axis lines", &showAxisLines)) {
      editorManager.getEditorGrid().setAxisLinesFlag(showAxisLines);
    }
  }
}

} // namespace liquidator

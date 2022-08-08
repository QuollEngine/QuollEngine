#include "liquid/core/Base.h"
#include "EditorGridPanel.h"

#include "liquid/imgui/Imgui.h"
#include "Widgets.h"

namespace liquidator {

void EditorGridPanel::render(EditorManager &editorManager) {
  if (widgets::MainMenuBar::begin()) {
    if (ImGui::BeginMenu("Editor")) {
      ImGui::MenuItem("Grid", nullptr, &mOpen);
      ImGui::EndMenu();
    }
    widgets::MainMenuBar::end();
  }

  if (!mOpen) {
    return;
  }

  if (widgets::FixedWindow::begin("Editor Grid", mOpen)) {
    bool showGridLines = editorManager.getEditorGrid().gridLinesShown();
    if (ImGui::Checkbox("Show grid lines", &showGridLines)) {
      editorManager.getEditorGrid().setGridLinesFlag(showGridLines);
    }

    bool showAxisLines = editorManager.getEditorGrid().axisLinesShown();
    if (ImGui::Checkbox("Show axis lines", &showAxisLines)) {
      editorManager.getEditorGrid().setAxisLinesFlag(showAxisLines);
    }
  }

  widgets::FixedWindow::end();
}

} // namespace liquidator

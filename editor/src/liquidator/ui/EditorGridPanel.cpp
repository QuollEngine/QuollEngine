#include "liquid/core/Base.h"
#include "EditorGridPanel.h"

#include "liquid/imgui/Imgui.h"
#include "Widgets.h"

namespace liquid::editor {

void EditorGridPanel::render(WorkspaceState &state) {
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
    bool showGridLines = state.grid.x == 1;
    if (ImGui::Checkbox("Show grid lines", &showGridLines)) {
      state.grid.x = static_cast<uint32_t>(showGridLines);
    }

    bool showAxisLines = state.grid.y == 1;
    if (ImGui::Checkbox("Show axis lines", &showAxisLines)) {
      state.grid.y = static_cast<uint32_t>(showAxisLines);
    }
  }
}

} // namespace liquid::editor

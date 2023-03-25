#include "liquid/core/Base.h"
#include "EditorGridPanel.h"

#include "liquid/imgui/Imgui.h"
#include "Widgets.h"

#include "liquidator/actions/EditorGridActions.h"

namespace liquid::editor {

void EditorGridPanel::render(WorkspaceState &state,
                             ActionExecutor &actionExecutor) {
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
    {
      bool enabled = SetGridLines::isShown(state);
      if (ImGui::Checkbox("Show grid lines", &enabled)) {
        actionExecutor.execute(std::make_unique<SetGridLines>(enabled));
      }
    }

    {
      bool enabled = SetGridAxisLines::isShown(state);
      if (ImGui::Checkbox("Show axis lines", &enabled)) {
        actionExecutor.execute(std::make_unique<SetGridAxisLines>(enabled));
      }
    }
  }
}

} // namespace liquid::editor

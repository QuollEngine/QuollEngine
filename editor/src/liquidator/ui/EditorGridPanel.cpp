#include "liquid/core/Base.h"
#include "EditorGridPanel.h"

#include "liquid/imgui/Imgui.h"
#include "Widgets.h"

#include "liquidator/actions/SetGridDataActions.h"

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
      bool enabled = SetGridLinesAction::isShown(state);
      if (ImGui::Checkbox("Show grid lines", &enabled)) {
        actionExecutor.execute(std::make_unique<SetGridLinesAction>(enabled));
      }
    }

    {
      bool enabled = SetGridAxisLinesAction::isShown(state);
      if (ImGui::Checkbox("Show axis lines", &enabled)) {
        actionExecutor.execute(
            std::make_unique<SetGridAxisLinesAction>(enabled));
      }
    }
  }
}

} // namespace liquid::editor

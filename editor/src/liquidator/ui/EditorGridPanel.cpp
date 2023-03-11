#include "liquid/core/Base.h"
#include "EditorGridPanel.h"

#include "liquid/imgui/Imgui.h"
#include "Widgets.h"

#include "liquidator/actions/SetGridDataActions.h"

namespace liquid::editor {

static const std::vector<Action> GridActions{SetGridLinesAction,
                                             SetGridAxisLinesAction};

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
    for (const auto &action : GridActions) {
      bool enabled = action.predicate(state);
      if (ImGui::Checkbox(String(action.name).c_str(), &enabled)) {
        actionExecutor.execute(action, enabled);
      }
    }
  }
}

} // namespace liquid::editor

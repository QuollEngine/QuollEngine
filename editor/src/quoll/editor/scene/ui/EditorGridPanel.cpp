#include "quoll/core/Base.h"
#include "EditorGridPanel.h"

#include "quoll/imgui/Imgui.h"

#include "quoll/editor/ui/Widgets.h"
#include "quoll/editor/actions/EditorGridActions.h"

namespace quoll::editor {

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
        actionExecutor.execute<SetGridLines>(enabled);
      }
    }

    {
      bool enabled = SetGridAxisLines::isShown(state);
      if (ImGui::Checkbox("Show axis lines", &enabled)) {
        actionExecutor.execute<SetGridAxisLines>(enabled);
      }
    }
  }
}

} // namespace quoll::editor

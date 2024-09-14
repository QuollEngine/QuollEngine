#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/editor/actions/EditorGridActions.h"
#include "quoll/editor/ui/Menu.h"
#include "quoll/editor/ui/MenuBar.h"
#include "quoll/editor/ui/Widgets.h"
#include "EditorGridPanel.h"

namespace quoll::editor {

void EditorGridPanel::renderMenu() {
  if (auto _ = MenuBar()) {
    if (auto editor = Menu("Editor")) {
      if (editor.item("Grid")) {
        mOpen = true;
      }
    }
  }
}

void EditorGridPanel::render(WorkspaceState &state,
                             ActionExecutor &actionExecutor) {
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

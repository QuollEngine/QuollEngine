#include "quoll/core/Base.h"
#include "quoll/editor/ui/StyleStack.h"
#include "quoll/editor/ui/Theme.h"

#include "WorkspaceTabs.h"

#include <imgui.h>

namespace quoll::editor {

void WorkspaceTabs::render(WorkspaceManager &workspaceManager) {
  StyleStack stack;
  stack.pushStyle(ImGuiStyleVar_FrameRounding, 0.0f);

  stack.pushColor(ImGuiCol_Button,
                  Theme::getColor(ThemeColor::MidnightBlack100));
  stack.pushColor(ImGuiCol_ButtonHovered,
                  Theme::getColor(ThemeColor::MidnightBlack200));
  stack.pushColor(ImGuiCol_ButtonActive,
                  Theme::getColor(ThemeColor::MidnightBlack200));

  for (size_t i = 0; i < workspaceManager.getWorkspaces().size(); ++i) {
    StyleStack itemStack;

    const auto &w = workspaceManager.getWorkspaces().at(i);

    if (i == workspaceManager.getCurrentWorkspaceIndex()) {
      itemStack.pushColor(ImGuiCol_Button,
                          Theme::getColor(ThemeColor::MidnightBlack200));
      itemStack.pushColor(ImGuiCol_ButtonHovered,
                          Theme::getColor(ThemeColor::MidnightBlack200));
      itemStack.pushColor(ImGuiCol_ButtonActive,
                          Theme::getColor(ThemeColor::MidnightBlack200));
    }

    if (ImGui::Button(w->getMatchParams().type.c_str())) {
      workspaceManager.switchWorkspace(i);
    }
  }
}

} // namespace quoll::editor

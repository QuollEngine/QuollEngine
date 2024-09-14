#include "quoll/core/Base.h"
#include "quoll/editor/ui/StyleStack.h"
#include "quoll/editor/ui/Theme.h"
#include "WorkspaceTabs.h"
#include <imgui.h>

namespace quoll::editor {

void WorkspaceTabs::render(WorkspaceManager &workspaceManager) {
  StyleStack stack;
  stack.pushStyle(ImGuiStyleVar_FrameRounding, 0.0f);

  stack.pushColor(ImGuiCol_Button, Theme::getColor(ThemeColor::Charcoal800));
  stack.pushColor(ImGuiCol_ButtonHovered,
                  Theme::getColor(ThemeColor::Charcoal700));
  stack.pushColor(ImGuiCol_ButtonActive,
                  Theme::getColor(ThemeColor::Charcoal700));

  for (size_t i = 0; i < workspaceManager.getWorkspaces().size(); ++i) {
    StyleStack itemStack;

    const auto &w = workspaceManager.getWorkspaces().at(i);

    if (i == workspaceManager.getCurrentWorkspaceIndex()) {
      itemStack.pushColor(ImGuiCol_Button,
                          Theme::getColor(ThemeColor::Charcoal700));
      itemStack.pushColor(ImGuiCol_ButtonHovered,
                          Theme::getColor(ThemeColor::Charcoal700));
      itemStack.pushColor(ImGuiCol_ButtonActive,
                          Theme::getColor(ThemeColor::Charcoal700));
    }

    if (ImGui::Button(w->getMatchParams().type.c_str())) {
      workspaceManager.switchWorkspace(i);
    }
  }
}

} // namespace quoll::editor

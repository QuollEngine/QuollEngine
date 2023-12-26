#include "quoll/core/Base.h"
#include "quoll/editor/ui/StyleStack.h"
#include "quoll/editor/ui/Theme.h"

#include "WorkspaceTabs.h"

#include <imgui.h>

namespace quoll::editor {

void WorkspaceTabs::render(WorkspaceManager &workspaceManager) {
  StyleStack stack;
  stack.pushColor(ImGuiCol_Tab, Theme::getColor(ThemeColor::Neutral100));
  stack.pushColor(ImGuiCol_TabActive, Theme::getColor(ThemeColor::Neutral300));
  stack.pushColor(ImGuiCol_TabHovered, Theme::getColor(ThemeColor::Neutral300));

  if (ImGui::BeginTabBar("Workspaces")) {
    for (const auto &w : workspaceManager.getWorkspaces()) {
      if (ImGui::BeginTabItem("Scene")) {
        ImGui::EndTabItem();
      }
    }
    ImGui::EndTabBar();
  }
}

} // namespace quoll::editor

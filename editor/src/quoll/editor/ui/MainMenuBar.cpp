#include "quoll/core/Base.h"

#include "MainMenuBar.h"
#include "Theme.h"

namespace quoll::editor {

MainMenuBar::MainMenuBar() {
  ImGui::PushStyleColor(ImGuiCol_MenuBarBg,
                        Theme::getColor(ThemeColor::MidnightBlack100));
  mExpanded = ImGui::BeginMainMenuBar();
}

MainMenuBar::~MainMenuBar() {
  if (mExpanded) {
    ImGui::EndMainMenuBar();
  }

  ImGui::PopStyleColor();
}

} // namespace quoll::editor

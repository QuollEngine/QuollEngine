#include "quoll/core/Base.h"
#include "quoll/imgui/Imgui.h"
#include "quoll/editor/ui/Menu.h"

namespace quoll::editor {

Menu::Menu(String label) { mOpen = ImGui::BeginMenu(label.c_str()); }

Menu::~Menu() {
  if (mOpen) {
    ImGui::EndMenu();
  }
}

bool Menu::item(String label, Shortcut shortcut) {
  return ImGui::MenuItem(label.c_str(),
                         shortcut ? shortcut.toString().c_str() : nullptr);
}

} // namespace quoll::editor

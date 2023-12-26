#include "quoll/core/Base.h"
#include "MenuBar.h"

#include "Theme.h"

namespace quoll::editor {

MenuBar::MenuBar() { mExpanded = ImGui::BeginMenuBar(); }

MenuBar::~MenuBar() {
  if (mExpanded) {
    ImGui::EndMenuBar();
  }
}

} // namespace quoll::editor

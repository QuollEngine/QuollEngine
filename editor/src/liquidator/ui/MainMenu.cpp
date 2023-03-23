#include "liquid/core/Base.h"
#include "MainMenu.h"

#include "Widgets.h"

namespace liquid::editor {

MainMenuItem::MainMenuItem(MainMenuItem *parent, String label, Action *action,
                           Shortcut shortcut)
    : mParent(parent), mLabel(label), mAction(action), mShortcut(shortcut) {}

MainMenuItem &MainMenuItem::begin(String label) {
  mChildren.push_back({this, label, {}, Shortcut{}});

  return mChildren.at(mChildren.size() - 1);
}

MainMenuItem &MainMenuItem::end() { return mParent ? *mParent : *this; }

MainMenuItem &MainMenuItem::add(String label, Action *action,
                                Shortcut shortcut) {
  mChildren.push_back({this, label, action, shortcut});

  return *this;
}

void MainMenuItem::render(ActionExecutor &actionExecutor) const {
  if (mChildren.empty()) {

    if (ImGui::MenuItem(mLabel.c_str(),
                        mShortcut ? mShortcut.toString().c_str() : nullptr)) {
      actionExecutor.execute(mAction);
    }
    return;
  }

  if (ImGui::BeginMenu(mLabel.c_str())) {
    for (auto &child : mChildren) {
      child.render(actionExecutor);
    }
    ImGui::EndMenu();
  }
}

MainMenu::MainMenu() : MainMenuItem(nullptr, "", {}, {}) {}

void MainMenu::render(ActionExecutor &actionExecutor) {
  if (auto _ = widgets::MainMenuBar()) {
    for (auto &child : getChildren()) {
      child.render(actionExecutor);
    }
  }
}

} // namespace liquid::editor

#include "liquid/core/Base.h"
#include "ShortcutsManager.h"

namespace liquid::editor {

void ShortcutsManager::add(Shortcut shortcut, Action *action) {
  mShortcuts.push_back(shortcut);
  mActions.push_back(std::unique_ptr<Action>(action));
}

void ShortcutsManager::process(int key, int mods,
                               ActionExecutor &actionExecutor) {
  for (size_t i = 0; i < mShortcuts.size(); ++i) {
    const auto &shortcut = mShortcuts.at(i);

    if (shortcut == Shortcut(key, mods)) {
      actionExecutor.execute(mActions.at(i));
      return;
    }
  }
}

} // namespace liquid::editor

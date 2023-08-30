#include "quoll/core/Base.h"
#include "ShortcutsManager.h"

namespace quoll::editor {

void ShortcutsManager::add(Shortcut shortcut, ActionCreator *actionCreator) {
  mShortcuts.push_back(shortcut);
  mActionCreators.push_back(std::unique_ptr<ActionCreator>(actionCreator));
}

void ShortcutsManager::process(int key, int mods,
                               ActionExecutor &actionExecutor) {
  for (size_t i = 0; i < mShortcuts.size(); ++i) {
    const auto &shortcut = mShortcuts.at(i);

    if (shortcut == Shortcut(key, mods)) {
      actionExecutor.execute(mActionCreators.at(i)->create());
      return;
    }
  }
}

} // namespace quoll::editor

#pragma once

#include "quoll/editor/actions/ActionCreator.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "Shortcut.h"

namespace quoll::editor {

class ShortcutsManager {
public:
  void add(Shortcut shortcut, ActionCreator *actionCreator);

  void process(int key, int mods, ActionExecutor &actionExecutor);

private:
  std::vector<Shortcut> mShortcuts;
  std::vector<std::unique_ptr<ActionCreator>> mActionCreators;
};

} // namespace quoll::editor

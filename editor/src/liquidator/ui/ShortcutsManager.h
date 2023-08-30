#pragma once

#include "liquidator/actions/ActionCreator.h"
#include "liquidator/actions/ActionExecutor.h"
#include "Shortcut.h"

namespace quoll::editor {

/**
 * @brief Shortcuts manager
 */
class ShortcutsManager {
public:
  /**
   * @brief Add shortcut
   *
   * @param shortcut Shortcut
   * @param actionCreator Action creator
   */
  void add(Shortcut shortcut, ActionCreator *actionCreator);

  /**
   * @brief Process input
   *
   * @param key Input keyboard key
   * @param mods Input keyboard mods
   * @param actionExecutor Action executor
   */
  void process(int key, int mods, ActionExecutor &actionExecutor);

private:
  std::vector<Shortcut> mShortcuts;
  std::vector<std::unique_ptr<ActionCreator>> mActionCreators;
};

} // namespace quoll::editor

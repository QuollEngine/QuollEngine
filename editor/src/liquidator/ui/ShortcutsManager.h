#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/actions/ActionExecutor.h"
#include "Shortcut.h"

namespace liquid::editor {

/**
 * @brief Shortcuts manager
 */
class ShortcutsManager {
public:
  /**
   * @brief Add shortcut
   *
   * @param shortcut Shortcut
   * @param action Action
   */
  void add(Shortcut shortcut, Action *action);

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
  std::vector<std::unique_ptr<Action>> mActions;
};

} // namespace liquid::editor

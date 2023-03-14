#pragma once

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/ActionExecutor.h"

namespace liquid::editor {

enum class ToolbarItemType { Toggleable, HideWhenInactive };

/**
 * @brief Toolbar widget
 *
 * Actions such as "Play"
 * is stored in this widget
 */
class Toolbar {
private:
  struct ToolbarItem {
    std::unique_ptr<Action> action;
    String label;
    String icon;
    ToolbarItemType type;
  };

public:
  /**
   * @brief Toolbar height
   */
  static constexpr float Height = 60.0f;

public:
  /**
   * @brief Render toolbar
   *
   * @param state Workspace state
   * @param actionExecutor Action executor
   */
  void render(WorkspaceState &state, ActionExecutor &actionExecutor);

  /**
   * @brief Add action
   *
   * @param action Action
   * @param label Button label
   * @param icon Button icon
   * @param type Item type
   */
  void add(Action *action, String label, String icon, ToolbarItemType type);

private:
  std::vector<ToolbarItem> mItems;
};

} // namespace liquid::editor

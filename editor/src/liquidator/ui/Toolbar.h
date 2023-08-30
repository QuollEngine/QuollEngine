#pragma once

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/ActionExecutor.h"
#include "liquidator/actions/ActionCreator.h"

namespace quoll::editor {

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
    std::unique_ptr<ActionCreator> actionCreator;
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
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void render(WorkspaceState &state, AssetRegistry &assetRegistry,
              ActionExecutor &actionExecutor);

  /**
   * @brief Add toolbar item
   *
   * @param actionCreator Action creator
   * @param label Button label
   * @param icon Button icon
   * @param type Item type
   */
  void add(ActionCreator *actionCreator, String label, String icon,
           ToolbarItemType type);

private:
  std::vector<ToolbarItem> mItems;
};

} // namespace quoll::editor

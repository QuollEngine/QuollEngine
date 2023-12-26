#pragma once

#include "EntityPanel.h"

namespace quoll::editor {

/**
 * @brief Inspector panel
 */
class Inspector {
private:
  struct Tab {
    String name;
    String icon;
    std::function<void(WorkspaceState &, AssetRegistry &, ActionExecutor &)>
        renderFn;
  };

public:
  /**
   * @brief Create inspector
   */
  Inspector();

  /**
   * @brief Render inspector
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void render(WorkspaceState &state, AssetRegistry &assetRegistry,
              ActionExecutor &actionExecutor);

private:
  EntityPanel mEntityPanel;
  usize mSelectedIndex = 0;

  std::vector<Tab> mTabs;
};

} // namespace quoll::editor

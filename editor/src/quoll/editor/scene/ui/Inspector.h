#pragma once

#include "EntityPanel.h"

namespace quoll::editor {

class Inspector {
private:
  struct Tab {
    String name;
    String icon;
    std::function<void(WorkspaceState &, AssetRegistry &, ActionExecutor &)>
        renderFn;
  };

public:
  Inspector();

  void render(WorkspaceState &state, AssetRegistry &assetRegistry,
              ActionExecutor &actionExecutor);

private:
  EntityPanel mEntityPanel;
  usize mSelectedIndex = 0;

  std::vector<Tab> mTabs;
};

} // namespace quoll::editor

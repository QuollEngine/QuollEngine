#pragma once

#include "quoll/asset/AssetCache.h"
#include "EntityPanel.h"

namespace quoll::editor {

class Inspector {
private:
  struct Tab {
    String name;
    String icon;
    std::function<void(WorkspaceState &, AssetCache &, ActionExecutor &)>
        renderFn;
  };

public:
  Inspector();

  void render(WorkspaceState &state, AssetCache &assetCache,
              ActionExecutor &actionExecutor);

private:
  EntityPanel mEntityPanel;
  usize mSelectedIndex = 0;

  std::vector<Tab> mTabs;
};

} // namespace quoll::editor

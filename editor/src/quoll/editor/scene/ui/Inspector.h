#pragma once

namespace quoll::editor {

class AssetManager;
class ActionExecutor;
struct WorkspaceState;

class Inspector {
private:
  struct Tab {
    String name;
    String icon;
    std::function<void(WorkspaceState &, AssetManager &, ActionExecutor &)>
        renderFn;
  };

public:
  Inspector();

  void render(WorkspaceState &state, AssetManager &assetManager,
              ActionExecutor &actionExecutor);

private:
  EntityPanel mEntityPanel;
  usize mSelectedIndex = 0;

  std::vector<Tab> mTabs;
};

} // namespace quoll::editor

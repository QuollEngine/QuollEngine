#pragma once

#include "quoll/editor/workspace/WorkspaceState.h"

namespace quoll {

class AssetCache;

}

namespace quoll::editor {

struct ActionExecutorResult {

  std::vector<Entity> entitiesToSave;

  std::vector<Entity> entitiesToDelete;

  bool saveScene = false;

  bool addToHistory = false;
};

class Action : NoCopyMove {
public:
  virtual ~Action() = default;

  virtual ActionExecutorResult onExecute(WorkspaceState &state,
                                         AssetCache &assetCache) = 0;

  virtual ActionExecutorResult onUndo(WorkspaceState &state,
                                      AssetCache &assetCache);

  virtual bool predicate(WorkspaceState &state, AssetCache &assetCache) = 0;
};

} // namespace quoll::editor

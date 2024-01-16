#pragma once

#include "quoll/editor/workspace/WorkspaceState.h"

namespace quoll::editor {

struct ActionExecutorResult {

  std::vector<Entity> entitiesToSave;

  std::vector<Entity> entitiesToDelete;

  bool saveScene = false;

  bool addToHistory = false;
};

class Action {
public:
  virtual ~Action() = default;

  Action() = default;
  Action(const Action &) = delete;
  Action(Action &&) = delete;
  Action &operator=(const Action &) = delete;
  Action &operator=(Action &&) = delete;

  virtual ActionExecutorResult onExecute(WorkspaceState &state,
                                         AssetRegistry &assetRegistry) = 0;

  virtual ActionExecutorResult onUndo(WorkspaceState &state,
                                      AssetRegistry &assetRegistry);

  virtual bool predicate(WorkspaceState &state,
                         AssetRegistry &assetRegistry) = 0;
};

} // namespace quoll::editor

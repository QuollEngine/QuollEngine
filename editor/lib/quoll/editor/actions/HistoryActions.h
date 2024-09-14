#pragma once

#include "quoll/editor/actions/ActionExecutor.h"

namespace quoll::editor {

class Undo : public Action {
public:
  Undo(ActionExecutor &actionExecutor);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  ActionExecutor &mActionExecutor;
};

class Redo : public Action {
public:
  Redo(ActionExecutor &actionExecutor);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  ActionExecutor &mActionExecutor;
};

} // namespace quoll::editor

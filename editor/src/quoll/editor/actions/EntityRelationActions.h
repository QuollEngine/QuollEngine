#pragma once

#include "Action.h"

namespace quoll::editor {

class EntityRemoveParent : public Action {
public:
  EntityRemoveParent(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  Entity mPreviousParent;
  std::ptrdiff_t mChildIndex = 0;
};

class EntitySetParent : public Action {
public:
  EntitySetParent(Entity entity, Entity parent);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  Entity mParent;
  Entity mPreviousParent;
};

} // namespace quoll::editor

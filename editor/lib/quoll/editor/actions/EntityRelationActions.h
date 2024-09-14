#pragma once

#include "Action.h"

namespace quoll::editor {

class EntityRemoveParent : public Action {
public:
  EntityRemoveParent(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  Entity mPreviousParent = Entity::Null;
  std::ptrdiff_t mChildIndex = 0;
};

class EntitySetParent : public Action {
public:
  EntitySetParent(Entity entity, Entity parent);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  Entity mParent;
  Entity mPreviousParent = Entity::Null;
};

} // namespace quoll::editor

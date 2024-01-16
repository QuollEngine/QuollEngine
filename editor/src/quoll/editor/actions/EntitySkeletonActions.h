#pragma once

#include "Action.h"

#include "quoll/skeleton/Skeleton.h"

namespace quoll::editor {

class EntityToggleSkeletonDebugBones : public Action {
public:
  EntityToggleSkeletonDebugBones(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
};

class EntityDeleteSkeleton : public Action {
public:
  EntityDeleteSkeleton(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  Skeleton mOldComponent;
  std::optional<SkeletonDebug> mOldSkeletonDebug;
};

} // namespace quoll::editor

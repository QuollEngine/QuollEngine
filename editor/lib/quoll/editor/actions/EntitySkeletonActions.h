#pragma once

#include "quoll/skeleton/Skeleton.h"
#include "Action.h"

namespace quoll::editor {

class EntityToggleSkeletonDebugBones : public Action {
public:
  EntityToggleSkeletonDebugBones(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
};

class EntityDeleteSkeleton : public Action {
public:
  EntityDeleteSkeleton(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  SkeletonAssetRef mOldComponent;
  std::optional<SkeletonDebug> mOldSkeletonDebug;
};

} // namespace quoll::editor

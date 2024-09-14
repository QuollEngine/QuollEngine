#pragma once

#include "quoll/scene/AutoAspectRatio.h"
#include "quoll/scene/PerspectiveLens.h"
#include "Action.h"

namespace quoll::editor {

class EntityCreatePerspectiveLens : public Action {
public:
  EntityCreatePerspectiveLens(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
};

class EntityDeletePerspectiveLens : public Action {
public:
  EntityDeletePerspectiveLens(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  PerspectiveLens mOldPerspectiveLens;
  std::optional<AutoAspectRatio> mOldAspectRatio;

  bool mIsActiveCamera = false;
  bool mIsStartingCamera = false;
};

} // namespace quoll::editor

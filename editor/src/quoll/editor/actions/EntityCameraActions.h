#pragma once

#include "Action.h"
#include "quoll/scene/AutoAspectRatio.h"
#include "quoll/scene/PerspectiveLens.h"

namespace quoll::editor {

class EntityCreatePerspectiveLens : public Action {
public:
  EntityCreatePerspectiveLens(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
};

class EntityDeletePerspectiveLens : public Action {
public:
  EntityDeletePerspectiveLens(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  PerspectiveLens mOldPerspectiveLens;
  std::optional<AutoAspectRatio> mOldAspectRatio;

  bool mIsActiveCamera = false;
  bool mIsStartingCamera = false;
};

} // namespace quoll::editor

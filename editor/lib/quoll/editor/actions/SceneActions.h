#pragma once

#include "Action.h"

namespace quoll::editor {

class SceneSetStartingCamera : public Action {
public:
  SceneSetStartingCamera(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  Entity mPreviousCamera = Entity::Null;
};

class SceneSetStartingEnvironment : public Action {
public:
  SceneSetStartingEnvironment(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  Entity mPreviousEnvironment = Entity::Null;
};

class SceneRemoveStartingEnvironment : public Action {
public:
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mPreviousEnvironment = Entity::Null;
};

} // namespace quoll::editor

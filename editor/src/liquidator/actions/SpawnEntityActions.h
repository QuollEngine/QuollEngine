#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Spawn empty entity at view action
 */
class SpawnEmptyEntityAtView : public Action {
public:
  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action undo
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  Entity mSpawnedEntity = Entity::Null;
};

/**
 * @brief Spawn prefab at camera view action
 */
class SpawnPrefabAtView : public Action {
public:
  /**
   * @brief Create action
   *
   * @param handle Prefab asset handle
   * @param camera Camera entity
   */
  SpawnPrefabAtView(PrefabAssetHandle handle, Entity camera);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action undo
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  PrefabAssetHandle mHandle;
  Entity mCamera;
  Entity mSpawnedRootEntity = Entity::Null;
};

} // namespace liquid::editor

#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Spawn prefab at given transform action
 */
class SpawnPrefabAtTransform : public Action {
public:
  /**
   * @brief Create action
   *
   * @param handle Prefab asset handle
   * @param transform Transform
   */
  SpawnPrefabAtTransform(PrefabAssetHandle handle, LocalTransform transform);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

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
  LocalTransform mTransform;
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
};

} // namespace liquid::editor

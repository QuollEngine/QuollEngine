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
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action undo
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

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
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action undo
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  PrefabAssetHandle mHandle;
  Entity mCamera;
  Entity mSpawnedRootEntity = Entity::Null;
};

/**
 * @brief Spawn sprite at camera view action
 */
class SpawnSpriteAtView : public Action {
public:
  /**
   * @brief Create action
   *
   * @param handle Texture asset handle
   * @param camera Camera entity
   */
  SpawnSpriteAtView(TextureAssetHandle handle, Entity camera);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action undo
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  TextureAssetHandle mHandle;
  Entity mCamera;
  Entity mSpawnedEntity = Entity::Null;
};

} // namespace liquid::editor

#pragma once

#include "Action.h"

namespace quoll::editor {

/**
 * @brief Create or update entity mesh action
 */
class EntitySetMesh : public Action {
public:
  /**
   * @brief Set mesh for entity
   *
   * @param entity Entity mesh
   * @param mesh Mesh handle
   */
  EntitySetMesh(Entity entity, MeshAssetHandle mesh);

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
   * @brief Action executor
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
  Entity mEntity;
  MeshAssetHandle mMesh;
  MeshAssetHandle mOldMesh = MeshAssetHandle::Null;
};

/**
 * @brief Delete entity mesh action
 */
class EntityDeleteMesh : public Action {
public:
  /**
   * @brief Delete mesh from entity
   *
   * @param entity Entity mesh
   */
  EntityDeleteMesh(Entity entity);

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
   * @brief Action executor
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
  Entity mEntity;
  MeshAssetHandle mOldMesh = MeshAssetHandle::Null;
};

} // namespace quoll::editor

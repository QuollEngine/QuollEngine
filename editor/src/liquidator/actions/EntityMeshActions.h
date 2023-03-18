#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Set mesh for entity
 */
class EntitySetMesh : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param handle Mesh asset handle
   */
  EntitySetMesh(Entity entity, MeshAssetHandle handle);

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
  Entity mEntity;
  MeshAssetHandle mHandle;
};

/**
 * @brief Set skinned mesh for entity
 */
class EntitySetSkinnedMesh : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param handle Skinned mesh asset handle
   */
  EntitySetSkinnedMesh(Entity entity, SkinnedMeshAssetHandle handle);

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
  Entity mEntity;
  SkinnedMeshAssetHandle mHandle;
};

} // namespace liquid::editor

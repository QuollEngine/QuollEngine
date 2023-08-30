#pragma once

#include "Action.h"
#include "EntityDefaultDeleteAction.h"

#include "liquid/renderer/SkinnedMeshRenderer.h"

namespace quoll::editor {

/**
 * @brief Set material slot for skinned mesh renderer
 */
class EntitySetSkinnedMeshRendererMaterial : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param slot Material slot
   * @param handle New material
   */
  EntitySetSkinnedMeshRendererMaterial(Entity entity, size_t slot,
                                       MaterialAssetHandle handle);

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
  Entity mEntity;
  size_t mSlot;
  MaterialAssetHandle mOldMaterial = MaterialAssetHandle::Null;
  MaterialAssetHandle mNewMaterial;
};

/**
 * @brief Add new material slot for skinned mesh renderer
 */
class EntityAddSkinnedMeshRendererMaterialSlot : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param handle New material
   */
  EntityAddSkinnedMeshRendererMaterialSlot(Entity entity,
                                           MaterialAssetHandle handle);

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
  Entity mEntity;
  MaterialAssetHandle mNewMaterial;
};

/**
 * @brief Remove last skinned mesh renderer material slot
 */
class EntityRemoveLastSkinnedMeshRendererMaterialSlot : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntityRemoveLastSkinnedMeshRendererMaterialSlot(Entity entity);

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
  Entity mEntity;
  MaterialAssetHandle mOldMaterial = MaterialAssetHandle::Null;
};

using EntityCreateSkinnedMeshRenderer =
    EntityDefaultCreateComponent<SkinnedMeshRenderer>;

using EntityDeleteSkinnedMeshRenderer =
    EntityDefaultDeleteAction<SkinnedMeshRenderer>;

} // namespace quoll::editor

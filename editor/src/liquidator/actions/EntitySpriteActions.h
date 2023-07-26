#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

namespace liquid::editor {

/**
 * @brief Set sprite for entity action
 */
class EntitySetSprite : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param handle Texture asset handle
   */
  EntitySetSprite(Entity entity, TextureAssetHandle handle);

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
  TextureAssetHandle mSprite;
  TextureAssetHandle mOldSprite = TextureAssetHandle::Null;
};

/**
 * @brief Create sprite entity action
 */
class EntityCreateSprite : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param handle Texture asset handle
   */
  EntityCreateSprite(Entity entity, TextureAssetHandle handle);

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
  TextureAssetHandle mHandle;
};

using EntityDeleteSprite = EntityDefaultDeleteAction<Sprite>;

} // namespace liquid::editor
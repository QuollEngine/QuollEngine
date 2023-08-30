#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

namespace quoll::editor {

/**
 * @brief Create animator entity action
 */
class EntityCreateAnimator : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param handle Animator asset handle
   */
  EntityCreateAnimator(Entity entity, AnimatorAssetHandle handle);

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
  AnimatorAssetHandle mHandle;
};

/**
 * @brief Set animator for entity action
 */
class EntitySetAnimator : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param script Animator handle
   */
  EntitySetAnimator(Entity entity, AnimatorAssetHandle script);

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
  AnimatorAssetHandle mAnimator;
  AnimatorAssetHandle mOldAnimator{};
};

using EntityDeleteAnimator = EntityDefaultDeleteAction<Animator>;

} // namespace quoll::editor

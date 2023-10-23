#pragma once

#include "Action.h"
#include "EntityDefaultDeleteAction.h"

#include "quoll/scene/LocalTransform.h"

namespace quoll::editor {

/**
 * @brief Entity set local transform continous action
 *
 * Continous actions are used when an action
 * has a start and end point.
 *
 * Example: Moving entity using gizmos
 */
class EntitySetLocalTransformContinuous : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param oldLocalTransform Old local transform
   * @param newLocalTransform New local transform
   */
  EntitySetLocalTransformContinuous(
      Entity entity, std::optional<LocalTransform> oldLocalTransform,
      std::optional<LocalTransform> newLocalTransform = std::nullopt);

  /**
   * @brief Set new local transform
   *
   * @param newLocalTransform New local transform
   */
  void setNewComponent(LocalTransform newLocalTransform);

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

  std::optional<LocalTransform> mOldLocalTransform;
  std::optional<LocalTransform> mNewLocalTransform;
};

} // namespace quoll::editor

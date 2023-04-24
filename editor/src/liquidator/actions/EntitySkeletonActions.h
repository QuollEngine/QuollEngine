#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

namespace liquid::editor {

/**
 * @brief Toggle debug bones for skeleton entity actions
 */
class EntityToggleSkeletonDebugBones : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntityToggleSkeletonDebugBones(Entity entity);

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
};

/**
 * @brief Delete skeleton from entity action
 */
class EntityDeleteSkeleton : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntityDeleteSkeleton(Entity entity);

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
  Entity mEntity;
  Skeleton mOldComponent;
  std::optional<SkeletonDebug> mOldSkeletonDebug;
};

} // namespace liquid::editor

#pragma once

#include "Action.h"

namespace liquid::editor {

/**
 * @brief Entity set local transform action
 */
class EntitySetLocalTransform : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param localTransform Local transform
   */
  EntitySetLocalTransform(Entity entity, LocalTransform localTransform);

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
  LocalTransform mLocalTransform;
};

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
   * @param localTransformStart Local transform start
   */
  EntitySetLocalTransformContinuous(Entity entity,
                                    LocalTransform localTransformStart);

  /**
   * @brief Set final local transform
   *
   * @param localTransformFinal Final local transform
   */
  void setLocalTransformFinal(LocalTransform localTransformFinal);

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

  LocalTransform mLocalTransformStart;
  std::optional<LocalTransform> mLocalTransformFinal;
};

} // namespace liquid::editor

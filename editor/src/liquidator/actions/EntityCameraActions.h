#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Set perspective lens for entity action
 */
class EntitySetPerspectiveLens : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param lens Perspective lens
   */
  EntitySetPerspectiveLens(Entity entity, PerspectiveLens lens);

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
  PerspectiveLens mLens;
};

/**
 * @brief Set camera auto aspect ratio for entity action
 */
class EntitySetCameraAutoAspectRatio : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntitySetCameraAutoAspectRatio(Entity entity);

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
 * @brief Set custom aspect ratio for entity action
 */
class EntitySetCameraCustomAspectRatio : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntitySetCameraCustomAspectRatio(Entity entity);

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

} // namespace liquid::editor
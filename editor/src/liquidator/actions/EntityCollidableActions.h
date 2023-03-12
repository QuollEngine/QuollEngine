#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Set collidable type for entity action
 */
class EntitySetCollidableType : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param type Collidable geometry type
   */
  EntitySetCollidableType(Entity entity, PhysicsGeometryType type);

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
  PhysicsGeometryType mType;
};

/**
 * @brief Set collidable for entity action
 */
class EntitySetCollidable : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param collidable Collidable
   */
  EntitySetCollidable(Entity entity, Collidable collidable);

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
  Collidable mCollidable;
};

} // namespace liquid::editor

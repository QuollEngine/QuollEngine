#pragma once

#include "Action.h"

namespace liquid::editor {

/**
 * @brief Set rigid body for entity action
 */
class EntitySetRigidBody : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param rigidBody Rigid body
   */
  EntitySetRigidBody(Entity entity, RigidBody rigidBody);

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
  RigidBody mRigidBody;
};

} // namespace liquid::editor

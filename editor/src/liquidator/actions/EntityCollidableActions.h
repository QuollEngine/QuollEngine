#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

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
  PhysicsGeometryType mType;
  Collidable mOldCollidable;
};

using EntitySetCollidable = EntityDefaultUpdateComponent<Collidable>;

using EntityCreateCollidable = EntityDefaultCreateComponent<Collidable>;

using EntityDeleteCollidable = EntityDefaultDeleteAction<Collidable>;

} // namespace liquid::editor

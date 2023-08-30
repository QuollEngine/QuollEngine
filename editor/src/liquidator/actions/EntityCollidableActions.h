#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

namespace quoll::editor {

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
  PhysicsGeometryType mType;
  Collidable mOldCollidable;
};

using EntitySetCollidable = EntityDefaultUpdateComponent<Collidable>;

using EntityCreateCollidable = EntityDefaultCreateComponent<Collidable>;

using EntityDeleteCollidable = EntityDefaultDeleteAction<Collidable>;

} // namespace quoll::editor

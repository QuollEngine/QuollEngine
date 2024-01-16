#pragma once

#include "Action.h"

#include "quoll/physics/Collidable.h"

namespace quoll::editor {

class EntitySetCollidableType : public Action {
public:
  EntitySetCollidableType(Entity entity, PhysicsGeometryType type);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  PhysicsGeometryType mType;
  Collidable mOldCollidable;
};

} // namespace quoll::editor

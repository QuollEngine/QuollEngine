#pragma once

#include "quoll/physics/Collidable.h"
#include "Action.h"

namespace quoll::editor {

class EntitySetCollidableType : public Action {
public:
  EntitySetCollidableType(Entity entity, PhysicsGeometryType type);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  PhysicsGeometryType mType;
  Collidable mOldCollidable;
};

} // namespace quoll::editor

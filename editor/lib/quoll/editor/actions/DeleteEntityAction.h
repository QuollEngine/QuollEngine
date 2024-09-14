#pragma once

#include "Action.h"

namespace quoll::editor {

class DeleteEntity : public Action {
public:
  DeleteEntity(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
};

} // namespace quoll::editor

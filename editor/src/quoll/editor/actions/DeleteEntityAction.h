#pragma once

#include "Action.h"

namespace quoll::editor {

class DeleteEntity : public Action {
public:
  DeleteEntity(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
};

} // namespace quoll::editor

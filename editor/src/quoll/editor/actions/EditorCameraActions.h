#pragma once

#include "Action.h"
#include "quoll/editor/scene/core/EditorCamera.h"

#include <glm/gtc/matrix_access.hpp>

namespace quoll::editor {

class MoveCameraToEntity : public Action {
public:
  MoveCameraToEntity(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
};

} // namespace quoll::editor

#pragma once

#include "quoll/editor/scene/core/EditorCamera.h"
#include "Action.h"
#include <glm/gtc/matrix_access.hpp>

namespace quoll::editor {

class MoveCameraToEntity : public Action {
public:
  MoveCameraToEntity(Entity entity);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
};

} // namespace quoll::editor

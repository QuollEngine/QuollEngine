#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/workspace/WorkspaceState.h"
#include "../core/EditorCamera.h"

namespace quoll::editor {

class SceneHierarchyPanel {
  using EntityClickHandler = std::function<void(Entity)>;

public:
  void render(WorkspaceState &state, ActionExecutor &actionExecutor);

private:
  void renderRoot(WorkspaceState &state, ActionExecutor &actionExecutor);

  u32 renderEntity(Entity entity, u32 index, int flags, WorkspaceState &state,
                   ActionExecutor &actionExecutor);

private:
  Entity mRightClickedEntity = Entity::Null;
};

} // namespace quoll::editor

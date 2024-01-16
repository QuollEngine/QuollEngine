#pragma once

#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/actions/EntityTransformActions.h"

namespace quoll::editor {

class SceneGizmos {
public:
  bool render(WorkspaceState &state, ActionExecutor &actionExecutor);

private:
  std::unique_ptr<EntitySetLocalTransformContinuous> mAction;
};

} // namespace quoll::editor

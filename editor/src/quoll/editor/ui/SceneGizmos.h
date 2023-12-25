#pragma once

#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/actions/EntityTransformActions.h"

namespace quoll::editor {

/**
 * @brief Scene gizmos
 *
 * Renders and controls gizmos in the scene
 */
class SceneGizmos {
public:
  /**
   * @brief Render scene gizmos
   *
   * @param state Workspace state
   * @param actionExecutor Action executor
   * @retval true Gizmo is hovered
   * @retval false Gizmo is not hovered
   */
  bool render(WorkspaceState &state, ActionExecutor &actionExecutor);

private:
  std::unique_ptr<EntitySetLocalTransformContinuous> mAction;
};

} // namespace quoll::editor

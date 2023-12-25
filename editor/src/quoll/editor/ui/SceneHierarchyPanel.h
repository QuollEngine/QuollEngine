#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/editor/editor-scene/EditorCamera.h"

#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor/actions/ActionExecutor.h"

namespace quoll::editor {

/**
 * @brief Scene hierarchy panel component
 */
class SceneHierarchyPanel {
  using EntityClickHandler = std::function<void(Entity)>;

public:
  /**
   * @brief Render the UI
   *
   * @param state Workspace state
   * @param actionExecutor Action executor
   */
  void render(WorkspaceState &state, ActionExecutor &actionExecutor);

private:
  /**
   * @brief Render scene root at top
   *
   * @param state Workspace state
   * @param actionExecutor Action executor
   */
  void renderRoot(WorkspaceState &state, ActionExecutor &actionExecutor);

  /**
   * @brief Render entity as tree item
   *
   * @param entity Entity
   * @param index Index
   * @param flags Flags
   * @param state Workspace state
   * @param actionExecutor Action executor
   * @return Calculated index
   */
  u32 renderEntity(Entity entity, u32 index, int flags, WorkspaceState &state,
                   ActionExecutor &actionExecutor);

private:
  Entity mRightClickedEntity = Entity::Null;
};

} // namespace quoll::editor

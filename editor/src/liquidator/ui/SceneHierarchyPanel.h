#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquidator/editor-scene/EditorCamera.h"

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/ActionExecutor.h"

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
  uint32_t renderEntity(Entity entity, uint32_t index, int flags,
                        WorkspaceState &state, ActionExecutor &actionExecutor);

private:
  Entity mRightClickedEntity = Entity::Null;
};

} // namespace quoll::editor

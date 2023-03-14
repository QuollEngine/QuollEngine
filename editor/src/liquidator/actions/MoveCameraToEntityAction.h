#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/editor-scene/EditorCamera.h"

#include <glm/gtc/matrix_access.hpp>

namespace liquid::editor {

/**
 * @brief Move camera to entity action
 */
class MoveCameraToEntityAction : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity to move the camera to
   */
  MoveCameraToEntityAction(Entity entity);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  Entity mEntity;
};

} // namespace liquid::editor

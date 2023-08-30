#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/editor-scene/EditorCamera.h"

#include <glm/gtc/matrix_access.hpp>

namespace quoll::editor {

/**
 * @brief Move camera to entity action
 */
class MoveCameraToEntity : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity to move the camera to
   */
  MoveCameraToEntity(Entity entity);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
};

} // namespace quoll::editor

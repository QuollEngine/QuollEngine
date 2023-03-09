#pragma once

#include "liquid/entity/Entity.h"
#include "liquidator/core/TransformOperation.h"

namespace liquid::editor {

/**
 * @brief State for editor workspace
 */
struct WorkspaceState {
  /**
   * Active transform operation
   */
  TransformOperation activeTransform = TransformOperation::Move;

  /**
   * Workspace camera
   */
  Entity camera{Entity::Null};

  /**
   * Selected entity
   */
  Entity selectedEntity{Entity::Null};

  /**
   * Grid in workspace
   *
   * First parameter is showing grid lines
   * Second parameter is showing axis lines
   */
  glm::uvec4 grid;
};

} // namespace liquid::editor

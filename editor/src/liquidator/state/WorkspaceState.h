#pragma once

#include "liquid/entity/Entity.h"
#include "liquid/scene/Scene.h"
#include "liquid/asset/AssetRegistry.h"
#include "liquidator/core/TransformOperation.h"
#include "liquidator/project/Project.h"

namespace liquid::editor {

enum class WorkspaceMode { Edit, Simulation };

/**
 * @brief State for editor workspace
 */
struct WorkspaceState {
  /**
   * Project data
   */
  Project project;

  /**
   * Asset registry
   */
  AssetRegistry &assetRegistry;

  /**
   * Workspace mode
   */
  WorkspaceMode mode = WorkspaceMode::Edit;

  /**
   * Scene
   */
  Scene scene;

  /**
   * Simulation scene
   */
  Scene simulationScene;

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

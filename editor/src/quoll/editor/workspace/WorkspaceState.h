#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/Entity.h"
#include "quoll/scene/Scene.h"
#include "quoll/editor/core/TransformOperation.h"
#include "quoll/editor/project/Project.h"

namespace quoll::editor {

enum class WorkspaceMode { Edit, Simulation };

struct WorkspaceState {
  Project project;

  WorkspaceMode mode = WorkspaceMode::Edit;

  Scene scene;

  TransformOperation activeTransform = TransformOperation::Move;

  Entity camera{Entity::Null};

  /**
   * Active camera
   *
   * This camera is passed to the renderer
   * and can point to either workspace camera,
   * scene's active camera, or any other camera.
   */
  Entity activeCamera{Entity::Null};

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

} // namespace quoll::editor

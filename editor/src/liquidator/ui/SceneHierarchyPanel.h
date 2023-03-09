#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquidator/editor-scene/EditorManager.h"
#include "liquidator/editor-scene/EditorCamera.h"

#include "liquidator/state/WorkspaceState.h"

namespace liquid::editor {

/**
 * @brief Scene hierarchy panel component
 */
class SceneHierarchyPanel {
  using EntityClickHandler = std::function<void(Entity)>;

public:
  /**
   * @brief Create scene hierarchy panel
   *
   * @param entityManager Entity manager
   */
  SceneHierarchyPanel(EntityManager &entityManager);

  /**
   * @brief Render the UI
   *
   * @param state Workspace state
   * @param editorManager Editor manager
   */
  void render(WorkspaceState &state, EditorManager &editorManager);

private:
  /**
   * @brief Render entity as tree item
   *
   * @param entity Entity
   * @param flags Flags
   * @param state Workspace state
   * @param editorManager Editor manager
   */
  void renderEntity(Entity entity, int flags, WorkspaceState &state,
                    EditorManager &editorManager);

private:
  EntityManager &mEntityManager;
  Entity mRightClickedEntity = Entity::Null;
};

} // namespace liquid::editor

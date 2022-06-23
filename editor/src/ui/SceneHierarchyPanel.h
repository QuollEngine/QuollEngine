#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "../editor-scene/EditorManager.h"
#include "../editor-scene/EditorCamera.h"

namespace liquidator {

/**
 * @brief Scene hierarchy panel component
 */
class SceneHierarchyPanel {
  using EntityClickHandler = std::function<void(liquid::Entity)>;

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
   * @param editorManager Editor manager
   */
  void render(EditorManager &editorManager);

  /**
   * @brief Set node click handler
   *
   * @param handler Node click handler
   */
  void setEntityClickHandler(const EntityClickHandler &handler);

private:
  /**
   * @brief Render entity as tree item
   *
   * @param entity Entity
   * @param flags Flags
   * @param editorManager Editor manager
   */
  void renderEntity(liquid::Entity entity, int flags,
                    EditorManager &editorManager);

private:
  EntityManager &mEntityManager;
  EntityClickHandler mEntityClickHandler;
  liquid::Entity mSelectedEntity = liquid::EntityNull;
};

} // namespace liquidator

#pragma once

#include "liquid/entity/EntityContext.h"
#include "../editor-scene/SceneManager.h"
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
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

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
   * @param sceneManager Scene manager
   */
  void renderEntity(liquid::Entity entity, int flags,
                    SceneManager &sceneManager);

private:
  EntityManager &mEntityManager;
  EntityClickHandler mEntityClickHandler;
  liquid::Entity mSelectedEntity = liquid::ENTITY_MAX;
};

} // namespace liquidator

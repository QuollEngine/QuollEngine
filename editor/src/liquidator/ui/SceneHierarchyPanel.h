#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquidator/editor-scene/EditorManager.h"
#include "liquidator/editor-scene/EditorCamera.h"

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
   * @brief Check if entity is selected
   *
   * @retval true Entity is selected
   * @retval false Entity is not selected
   */
  inline bool isEntitySelected() const {
    return mEntityManager.getActiveEntityDatabase().hasEntity(mSelectedEntity);
  }

  /**
   * @brief Get selected entity
   *
   * @return Selected entity
   */
  inline const liquid::Entity getSelectedEntity() const {
    return mSelectedEntity;
  }

  /**
   * @brief Set selected entity
   *
   * @param entity Entity
   */
  void setSelectedEntity(liquid::Entity entity);

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
  liquid::Entity mSelectedEntity = liquid::EntityNull;
  liquid::Entity mRightClickedEntity = liquid::EntityNull;
};

} // namespace liquidator

#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquidator/editor-scene/EditorManager.h"
#include "liquidator/editor-scene/EditorCamera.h"

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
    return mEntityManager.getActiveEntityDatabase().exists(mSelectedEntity);
  }

  /**
   * @brief Get selected entity
   *
   * @return Selected entity
   */
  inline const Entity getSelectedEntity() const { return mSelectedEntity; }

  /**
   * @brief Set selected entity
   *
   * @param entity Entity
   */
  void setSelectedEntity(Entity entity);

private:
  /**
   * @brief Render entity as tree item
   *
   * @param entity Entity
   * @param flags Flags
   * @param editorManager Editor manager
   */
  void renderEntity(Entity entity, int flags, EditorManager &editorManager);

private:
  EntityManager &mEntityManager;
  Entity mSelectedEntity = EntityNull;
  Entity mRightClickedEntity = EntityNull;
};

} // namespace liquid::editor

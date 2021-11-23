#pragma once

#include "entity/EntityContext.h"
#include "../editor-scene/SceneManager.h"

namespace liquidator {

class EntityPanel {
public:
  /**
   * @brief Create entity panel
   *
   * @param entityContext Entity context
   */
  EntityPanel(liquid::EntityContext &entityContext);

  /**
   * @brief Render panel
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

  /**
   * @brief Set selected entity
   *
   * @param entity Entity to select
   */
  void setSelectedEntity(liquid::Entity entity);

private:
  /**
   * @brief Render name component
   */
  void renderName();

  /**
   * @brief Render light component
   */
  void renderLight();

  /**
   * @brief Render transform component
   */
  void renderTransform();

private:
  liquid::Entity selectedEntity = liquid::ENTITY_MAX;
  liquid::EntityContext &context;
};

} // namespace liquidator

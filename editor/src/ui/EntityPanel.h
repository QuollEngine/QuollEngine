#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/animation/AnimationSystem.h"
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
   * @param animationSystem Animation system
   */
  void render(SceneManager &sceneManager,
              const liquid::AnimationSystem &animationSystem);

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

  /**
   * @brief Render animation component
   *
   * @param animationSystem Animation system
   */
  void renderAnimation(const liquid::AnimationSystem &animationSystem);

private:
  liquid::Entity selectedEntity = liquid::ENTITY_MAX;
  liquid::EntityContext &context;
};

} // namespace liquidator

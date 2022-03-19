#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"

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
   * @param physicsSystem Physics system
   */
  void render(SceneManager &sceneManager,
              const liquid::AnimationSystem &animationSystem,
              liquid::PhysicsSystem &physicsSystem);

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

  /**
   * @brief Render skeleton component
   */
  void renderSkeleton();

  /**
   * @brief Render collidable component
   */
  void renderCollidable();

  /**
   * @brief Render rigid body component
   */
  void renderRigidBody();

  /**
   * @brief Render add component button
   */
  void renderAddComponent();

private:
  liquid::Entity mSelectedEntity = liquid::ENTITY_MAX;
  liquid::EntityContext &mEntityContext;
};

} // namespace liquidator

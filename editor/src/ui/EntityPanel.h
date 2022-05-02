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
   * @param entityManager Entity manager
   */
  EntityPanel(liquid::EntityContext &entityContext,
              EntityManager &entityManager);

  /**
   * @brief Render panel
   *
   * @param sceneManager Scene manager
   * @param renderer Renderer
   * @param assetRegistry Asset registry
   * @param physicsSystem Physics system
   */
  void render(SceneManager &sceneManager, liquid::Renderer &renderer,
              liquid::AssetRegistry &assetRegistry,
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
   * @brief Render camera component
   */
  void renderCamera();

  /**
   * @brief Render transform component
   */
  void renderTransform();

  /**
   * @brief Render mesh
   *
   * @param assetRegistry Asset registry
   */
  void renderMesh(liquid::AssetRegistry &assetRegistry);

  /**
   * @brief Render animation component
   *
   * @param assetRegistry Asset registry
   */
  void renderAnimation(liquid::AssetRegistry &assetRegistry);

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

  /**
   * @brief Handle drag and drop
   *
   * @param renderer Renderer
   * @param assetRegistry Asset registry
   * @param sceneManager Scene manager
   */
  void handleDragAndDrop(liquid::Renderer &renderer,
                         liquid::AssetRegistry &assetRegistry,
                         SceneManager &sceneManager);

private:
  liquid::Entity mSelectedEntity = liquid::ENTITY_MAX;
  liquid::EntityContext &mEntityContext;
  liquid::String mName;
  EntityManager &mEntityManager;
};

} // namespace liquidator

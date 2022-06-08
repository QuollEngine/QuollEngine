#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"

#include "../editor-scene/EditorManager.h"

namespace liquidator {

/**
 * @brief Entity panel component
 */
class EntityPanel {
public:
  /**
   * @brief Create entity panel
   *
   * @param entityManager Entity manager
   */
  EntityPanel(EntityManager &entityManager);

  /**
   * @brief Render panel
   *
   * @param editorManager Editor manager
   * @param renderer Renderer
   * @param assetRegistry Asset registry
   * @param physicsSystem Physics system
   */
  void render(EditorManager &editorManager, liquid::Renderer &renderer,
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
   *
   * @param editorManager Editor manager
   */
  void renderCamera(EditorManager &editorManager);

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
   * @brief Render scripting component
   *
   * @param assetRegistry Asset registry
   */
  void renderScripting(liquid::AssetRegistry &assetRegistry);

  /**
   * @brief Render add component button
   */
  void renderAddComponent();

  /**
   * @brief Handle drag and drop
   *
   * @param renderer Renderer
   * @param assetRegistry Asset registry
   */
  void handleDragAndDrop(liquid::Renderer &renderer,
                         liquid::AssetRegistry &assetRegistry);

private:
  liquid::Entity mSelectedEntity = liquid::EntityNull;
  liquid::String mName;
  EntityManager &mEntityManager;
  bool mIsNameActivated = false;
};

} // namespace liquidator

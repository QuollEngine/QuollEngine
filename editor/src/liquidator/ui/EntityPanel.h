#pragma once

#include "liquid/entity/EntityDatabase.h"
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
   * @param entity Entity to display
   * @param renderer Renderer
   * @param assetRegistry Asset registry
   * @param physicsSystem Physics system
   */
  void render(EditorManager &editorManager, liquid::Entity entity,
              liquid::Renderer &renderer, liquid::AssetRegistry &assetRegistry,
              liquid::PhysicsSystem &physicsSystem);

private:
  /**
   * @brief Set selected entity
   *
   * @param entity Entity to select
   */
  void setSelectedEntity(liquid::Entity entity);

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
   * @brief Render text component
   *
   * @param assetRegistry Asset registry
   */
  void renderText(liquid::AssetRegistry &assetRegistry);

  /**
   * @brief Render audio component
   *
   * @param assetRegistry Asset registry
   */
  void renderAudio(liquid::AssetRegistry &assetRegistry);

  /**
   * @brief Render scripting component
   *
   * @param assetRegistry Asset registry
   */
  void renderScripting(liquid::AssetRegistry &assetRegistry);

  /**
   * @brief Render add component button
   *
   * @param assetRegistry Asset registry
   */
  void renderAddComponent(liquid::AssetRegistry &assetRegistry);

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

#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"

#include "../editor-scene/EditorManager.h"

namespace liquid::editor {

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
   * @param assetManager Asset manager
   * @param physicsSystem Physics system
   */
  void render(EditorManager &editorManager, Entity entity, Renderer &renderer,
              AssetManager &assetManager, PhysicsSystem &physicsSystem);

private:
  /**
   * @brief Set selected entity
   *
   * @param entity Entity to select
   */
  void setSelectedEntity(Entity entity);

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
  void renderMesh(AssetRegistry &assetRegistry);

  /**
   * @brief Render animation component
   *
   * @param assetRegistry Asset registry
   */
  void renderAnimation(AssetRegistry &assetRegistry);

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
  void renderText(AssetRegistry &assetRegistry);

  /**
   * @brief Render audio component
   *
   * @param assetRegistry Asset registry
   */
  void renderAudio(AssetRegistry &assetRegistry);

  /**
   * @brief Render scripting component
   *
   * @param assetRegistry Asset registry
   */
  void renderScripting(AssetRegistry &assetRegistry);

  /**
   * @brief Render add component button
   *
   * @param assetRegistry Asset registry
   */
  void renderAddComponent(AssetRegistry &assetRegistry);

  /**
   * @brief Handle drag and drop
   *
   * @param renderer Renderer
   * @param assetRegistry Asset registry
   */
  void handleDragAndDrop(Renderer &renderer, AssetRegistry &assetRegistry);

private:
  Entity mSelectedEntity = EntityNull;
  String mName;
  EntityManager &mEntityManager;
  bool mIsNameActivated = false;
};

} // namespace liquid::editor

#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"

#include "../editor-scene/EditorManager.h"

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/ActionExecutor.h"
#include "liquidator/actions/EntityScriptingActions.h"

namespace liquid::editor {

/**
 * @brief Entity panel component
 */
class EntityPanel {
public:
  /**
   * @brief Render panel
   *
   * @param state Workspace state
   * @param actionExecutor Action executor
   * @param entity Entity to display
   */
  void render(WorkspaceState &state, ActionExecutor &actionExecutor,
              Entity entity);

private:
  /**
   * @brief Set selected entity
   *
   * @param scene Scene
   * @param entity Entity to select
   */
  void setSelectedEntity(Scene &scene, Entity entity);

  /**
   * @brief Render name component
   *
   * @param scene Scene
   * @param actionExecutor Action executor
   */
  void renderName(Scene &scene, ActionExecutor &actionExecutor);

  /**
   * @brief Render light component
   *
   * @param scene Active scene
   * @param actionExecutor Action executor
   */
  void renderLight(Scene &scene, ActionExecutor &actionExecutor);

  /**
   * @brief Render directional light component
   *
   * @param scene Active scene
   * @param actionExecutor Action executor
   */
  void renderDirectionalLight(Scene &scene, ActionExecutor &actionExecutor);

  /**
   * @brief Render point light component
   *
   * @param scene Active scene
   * @param actionExecutor Action executor
   */
  void renderPointLight(Scene &scene, ActionExecutor &actionExecutor);

  /**
   * @brief Render camera component
   *
   * @param state Workspace state
   * @param scene Scene
   * @param actionExecutor Action executor
   */
  void renderCamera(WorkspaceState &state, Scene &scene,
                    ActionExecutor &actionExecutor);

  /**
   * @brief Render transform component
   *
   * @param scene Scene
   * @param actionExecutor Action executor
   */
  void renderTransform(Scene &scene, ActionExecutor &actionExecutor);

  /**
   * @brief Render mesh
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   */
  void renderMesh(Scene &scene, AssetRegistry &assetRegistry);

  /**
   * @brief Render animation component
   *
   * @param state Workspace state
   * @param scene Scene
   * @param assetRegistry Asset registry
   */
  void renderAnimation(WorkspaceState &state, Scene &scene,
                       AssetRegistry &assetRegistry);

  /**
   * @brief Render skeleton component
   *
   * @param scene Scene
   * @param actionExecutor Action executor
   */
  void renderSkeleton(Scene &scene, ActionExecutor &actionExecutor);

  /**
   * @brief Render collidable component
   *
   * @param scene Scene
   * @param actionExecutor Action executor
   */
  void renderCollidable(Scene &scene, ActionExecutor &actionExecutor);

  /**
   * @brief Render rigid body component
   *
   * @param scene Scene
   * @param actionExecutor Action executor
   */
  void renderRigidBody(Scene &scene, ActionExecutor &actionExecutor);

  /**
   * @brief Render text component
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void renderText(Scene &scene, AssetRegistry &assetRegistry,
                  ActionExecutor &actionExecutor);

  /**
   * @brief Render audio component
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   */
  void renderAudio(Scene &scene, AssetRegistry &assetRegistry);

  /**
   * @brief Render scripting component
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void renderScripting(Scene &scene, AssetRegistry &assetRegistry,
                       ActionExecutor &actionExecutor);

  /**
   * @brief Render add component button
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void renderAddComponent(Scene &scene, AssetRegistry &assetRegistry,
                          ActionExecutor &actionExecutor);

  /**
   * @brief Handle drag and drop
   *
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void handleDragAndDrop(AssetRegistry &assetRegistry,
                         ActionExecutor &actionExecutor);

private:
  Entity mSelectedEntity = Entity::Null;
  bool mIsNameActivated = false;

  Name mName;

private:
  std::optional<RigidBody> mRigidBody;
  std::optional<LocalTransform> mLocalTransform;
  std::optional<Text> mText;
  std::optional<Collidable> mCollidable;
  std::optional<DirectionalLight> mDirectionalLight;
  std::optional<PointLight> mPointLight;
  std::optional<PerspectiveLens> mPerspectiveLens;
  std::optional<CascadedShadowMap> mCascadedShadowMap;
  std::unique_ptr<EntitySetScriptVariable> mSetScriptVariable;
};

} // namespace liquid::editor

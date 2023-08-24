#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/ActionExecutor.h"
#include "liquidator/actions/EntityScriptingActions.h"
#include "liquidator/actions/EntityNameActions.h"
#include "liquidator/actions/EntityCollidableActions.h"
#include "liquidator/actions/EntityRigidBodyActions.h"
#include "liquidator/actions/EntityTextActions.h"
#include "liquidator/actions/EntityLightActions.h"
#include "liquidator/actions/EntityCameraActions.h"

namespace liquid::editor {

/**
 * @brief Entity panel component
 */
class EntityPanel {
public:
  /**
   * @brief Render entity panel
   *
   * @param state Workspace state
   * @param actionExecutor Action executor
   * @param assetRegistry Asset registry
   */
  void renderContent(WorkspaceState &state, AssetRegistry &assetRegistry,
                     ActionExecutor &actionExecutor);

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
   * @brief Render sprite
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void renderSprite(Scene &scene, AssetRegistry &assetRegistry,
                    ActionExecutor &actionExecutor);

  /**
   * @brief Render mesh
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void renderMesh(Scene &scene, AssetRegistry &assetRegistry,
                  ActionExecutor &actionExecutor);

  /**
   * @brief Render mesh renderer
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void renderMeshRenderer(Scene &scene, AssetRegistry &assetRegistry,
                          ActionExecutor &actionExecutor);

  /**
   * @brief Render skinned mesh renderer
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void renderSkinnedMeshRenderer(Scene &scene, AssetRegistry &assetRegistry,
                                 ActionExecutor &actionExecutor);

  /**
   * @brief Render animation component
   *
   * @param state Workspace state
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void renderAnimation(WorkspaceState &state, Scene &scene,
                       AssetRegistry &assetRegistry,
                       ActionExecutor &actionExecutor);

  /**
   * @brief Render skeleton component
   *
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void renderSkeleton(Scene &scene, AssetRegistry &assetRegistry,
                      ActionExecutor &actionExecutor);

  /**
   * @brief Render joint attachment component
   *
   * @param scene Scene
   * @param actionExecutor Action executor
   */
  void renderJointAttachment(Scene &scene, ActionExecutor &actionExecutor);

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
   * @param actionExecutor Action executor
   */
  void renderAudio(Scene &scene, AssetRegistry &assetRegistry,
                   ActionExecutor &actionExecutor);

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
   * @param scene Scene
   * @param assetRegistry Asset registry
   * @param actionExecutor Action executor
   */
  void handleDragAndDrop(Scene &scene, AssetRegistry &assetRegistry,
                         ActionExecutor &actionExecutor);

  /**
   * @brief Check if item should be deleted
   *
   * @param component Component name
   * @retval true Item should be deleted
   * @retval false Item should not be deleted
   */
  bool shouldDelete(const char *component);

private:
  Entity mSelectedEntity = Entity::Null;
  bool mIsNameActivated = false;

private:
  std::unique_ptr<EntitySetName> mNameAction;
  std::unique_ptr<EntitySetLocalTransformContinuous> mLocalTransformAction;
  std::unique_ptr<EntitySetCollidable> mCollidableAction;
  std::unique_ptr<EntitySetRigidBody> mRigidBodyAction;
  std::unique_ptr<EntitySetText> mTextAction;
  std::unique_ptr<EntitySetDirectionalLight> mDirectionalLightAction;
  std::unique_ptr<EntitySetCascadedShadowMap> mCascadedShadowMapAction;
  std::unique_ptr<EntitySetScriptVariable> mSetScriptVariable;
  std::unique_ptr<EntitySetPointLight> mPointLightAction;
  std::unique_ptr<EntitySetPerspectiveLens> mPerspectiveLensAction;
};

} // namespace liquid::editor

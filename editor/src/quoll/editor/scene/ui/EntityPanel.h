#pragma once

#include "quoll/core/Name.h"
#include "quoll/animation/AnimationSystem.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/physics/Collidable.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/physics/RigidBody.h"
#include "quoll/scene/CascadedShadowMap.h"
#include "quoll/scene/EnvironmentSkybox.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/text/Text.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/actions/EntityCameraActions.h"
#include "quoll/editor/actions/EntityCollidableActions.h"
#include "quoll/editor/actions/EntityLightActions.h"
#include "quoll/editor/actions/EntityScriptingActions.h"
#include "quoll/editor/actions/EntityTransformActions.h"
#include "quoll/editor/actions/EntityUpdateComponentAction.h"
#include "quoll/editor/workspace/WorkspaceState.h"

namespace quoll::editor {

class EntityPanel {
public:
  void renderContent(WorkspaceState &state, AssetRegistry &assetRegistry,
                     ActionExecutor &actionExecutor);

private:
  void setSelectedEntity(Scene &scene, Entity entity);

  void renderName(Scene &scene, ActionExecutor &actionExecutor);

  void renderDirectionalLight(Scene &scene, ActionExecutor &actionExecutor);

  void renderPointLight(Scene &scene, ActionExecutor &actionExecutor);

  void renderCamera(WorkspaceState &state, Scene &scene,
                    ActionExecutor &actionExecutor);

  void renderTransform(Scene &scene, ActionExecutor &actionExecutor);

  void renderSprite(Scene &scene, AssetRegistry &assetRegistry,
                    ActionExecutor &actionExecutor);

  void renderUICanvas(Scene &scene, ActionExecutor &actionExecutor);

  void renderMesh(Scene &scene, AssetRegistry &assetRegistry,
                  ActionExecutor &actionExecutor);

  void renderMeshRenderer(Scene &scene, AssetRegistry &assetRegistry,
                          ActionExecutor &actionExecutor);

  void renderSkinnedMeshRenderer(Scene &scene, AssetRegistry &assetRegistry,
                                 ActionExecutor &actionExecutor);

  void renderAnimation(WorkspaceState &state, Scene &scene,
                       AssetRegistry &assetRegistry,
                       ActionExecutor &actionExecutor);

  void renderSkeleton(Scene &scene, AssetRegistry &assetRegistry,
                      ActionExecutor &actionExecutor);

  void renderJointAttachment(Scene &scene, ActionExecutor &actionExecutor);

  void renderCollidable(Scene &scene, ActionExecutor &actionExecutor);

  void renderRigidBody(Scene &scene, ActionExecutor &actionExecutor);

  void renderText(Scene &scene, AssetRegistry &assetRegistry,
                  ActionExecutor &actionExecutor);

  void renderAudio(Scene &scene, AssetRegistry &assetRegistry,
                   ActionExecutor &actionExecutor);

  void renderScripting(Scene &scene, AssetRegistry &assetRegistry,
                       ActionExecutor &actionExecutor);

  void renderSkybox(Scene &scene, AssetRegistry &assetRegistry,
                    ActionExecutor &actionExecutor);

  void renderInput(Scene &scene, AssetRegistry &assetRegistry,
                   ActionExecutor &actionExecutor);

  void renderEnvironmentLighting(Scene &scene, AssetRegistry &assetRegistry,
                                 ActionExecutor &actionExecutor);

  void renderDebug();

  void renderAddComponent(Scene &scene, AssetRegistry &assetRegistry,
                          ActionExecutor &actionExecutor);

  void handleDragAndDrop(Scene &scene, AssetRegistry &assetRegistry,
                         ActionExecutor &actionExecutor);

  bool shouldDelete(const char *component);

private:
  Entity mSelectedEntity = Entity::Null;
  bool mIsNameActivated = false;

private:
  std::unique_ptr<EntityUpdateComponent<Name>> mNameAction;
  std::unique_ptr<EntitySetLocalTransformContinuous> mLocalTransformAction;
  std::unique_ptr<EntityUpdateComponent<Collidable>> mCollidableAction;
  std::unique_ptr<EntityUpdateComponent<RigidBody>> mRigidBodyAction;
  std::unique_ptr<EntityUpdateComponent<Text>> mTextAction;
  std::unique_ptr<EntityUpdateComponent<DirectionalLight>>
      mDirectionalLightAction;
  std::unique_ptr<EntityUpdateComponent<CascadedShadowMap>>
      mCascadedShadowMapAction;
  std::unique_ptr<EntitySetScriptVariable> mSetScriptVariable;
  std::unique_ptr<EntityUpdateComponent<PointLight>> mPointLightAction;
  std::unique_ptr<EntityUpdateComponent<PerspectiveLens>>
      mPerspectiveLensAction;
  std::unique_ptr<EntityUpdateComponent<EnvironmentSkybox>>
      mEnvironmentSkyboxAction;
};

} // namespace quoll::editor

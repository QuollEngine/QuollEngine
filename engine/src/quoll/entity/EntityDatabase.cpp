#include "quoll/core/Base.h"

#include "quoll/core/Name.h"
#include "quoll/core/Id.h"
#include "quoll/core/Delete.h"
#include "quoll/scene/Mesh.h"
#include "quoll/scene/SkinnedMesh.h"
#include "quoll/scene/DirectionalLight.h"
#include "quoll/scene/PointLight.h"
#include "quoll/scene/CascadedShadowMap.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/AutoAspectRatio.h"
#include "quoll/scene/Skeleton.h"
#include "quoll/scene/JointAttachment.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/scene/EnvironmentSkybox.h"
#include "quoll/scene/EnvironmentLighting.h"
#include "quoll/scene/Sprite.h"
#include "quoll/animation/Animator.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll/audio/AudioSource.h"
#include "quoll/audio/AudioStart.h"
#include "quoll/audio/AudioStatus.h"
#include "quoll/physics/RigidBody.h"
#include "quoll/physics/Collidable.h"
#include "quoll/physics/Force.h"
#include "quoll/physics/Torque.h"
#include "quoll/physics/Impulse.h"
#include "quoll/physics/RigidBodyClear.h"
#include "quoll/text/Text.h"
#include "quoll/renderer/MeshRenderer.h"
#include "quoll/renderer/SkinnedMeshRenderer.h"
#include "quoll/input/InputMap.h"
#include "quoll/physx/PhysxInstance.h"
#include "quoll/scripting/Script.h"
#include "quoll/ui/UICanvas.h"
#include "quoll/ui/UICanvasRenderRequest.h"

#include "EntityDatabase.h"

namespace quoll {

EntityDatabase::EntityDatabase() {
  reg<Id>();
  reg<Name>();
  reg<Delete>();
  reg<Mesh>();
  reg<Sprite>();
  reg<DirectionalLight>();
  reg<PointLight>();
  reg<CascadedShadowMap>();
  reg<Camera>();
  reg<AutoAspectRatio>();
  reg<PerspectiveLens>();
  reg<LocalTransform>();
  reg<WorldTransform>();
  reg<Parent>();
  reg<Children>();
  reg<EnvironmentSkybox>();
  reg<EnvironmentLightingSkyboxSource>();
  reg<Animator>();
  reg<AnimatorEvent>();
  reg<AudioSource>();
  reg<AudioStart>();
  reg<AudioStatus>();
  reg<SkinnedMesh>();
  reg<Skeleton>();
  reg<JointAttachment>();
  reg<SkeletonDebug>();
  reg<RigidBody>();
  reg<Collidable>();
  reg<PhysxInstance>();
  reg<Force>();
  reg<Impulse>();
  reg<Torque>();
  reg<RigidBodyClear>();
  reg<Script>();
  reg<Text>();
  reg<MeshRenderer>();
  reg<SkinnedMeshRenderer>();
  reg<InputMapAssetRef>();
  reg<InputMap>();
  reg<UICanvas>();
  reg<UICanvasRenderRequest>();
}

} // namespace quoll

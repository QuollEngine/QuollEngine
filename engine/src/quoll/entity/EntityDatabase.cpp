#include "quoll/core/Base.h"
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
  reg<Torque>();
  reg<RigidBodyClear>();
  reg<Script>();
  reg<Text>();
  reg<MeshRenderer>();
  reg<SkinnedMeshRenderer>();
  reg<InputMapAssetRef>();
  reg<InputMap>();
}

} // namespace quoll

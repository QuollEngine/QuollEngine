#include "liquid/core/Base.h"
#include "EntityDatabase.h"

namespace liquid {

EntityDatabase::EntityDatabase() {
  reg<Id>();
  reg<Name>();
  reg<Delete>();
  reg<Mesh>();
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
  reg<AudioSource>();
  reg<AudioStart>();
  reg<AudioStatus>();
  reg<SkinnedMesh>();
  reg<Skeleton>();
  reg<SkeletonDebug>();
  reg<RigidBody>();
  reg<Collidable>();
  reg<Force>();
  reg<Torque>();
  reg<RigidBodyClear>();
  reg<Script>();
  reg<Text>();
}

} // namespace liquid

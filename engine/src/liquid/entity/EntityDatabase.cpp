#include "liquid/core/Base.h"
#include "EntityDatabase.h"

namespace liquid {

EntityDatabase::EntityDatabase() {
  reg<IdComponent>();
  reg<NameComponent>();
  reg<DeleteComponent>();
  reg<MeshComponent>();
  reg<DirectionalLightComponent>();
  reg<CameraComponent>();
  reg<AutoAspectRatioComponent>();
  reg<PerspectiveLensComponent>();
  reg<LocalTransformComponent>();
  reg<WorldTransformComponent>();
  reg<ParentComponent>();
  reg<ChildrenComponent>();
  reg<EnvironmentComponent>();
  reg<AnimatorComponent>();
  reg<AudioSourceComponent>();
  reg<AudioStartComponent>();
  reg<AudioStatusComponent>();
  reg<SkinnedMeshComponent>();
  reg<SkeletonComponent>();
  reg<SkeletonDebugComponent>();
  reg<RigidBodyComponent>();
  reg<CollidableComponent>();
  reg<ForceComponent>();
  reg<TorqueComponent>();
  reg<RigidBodyClearComponent>();
  reg<ScriptingComponent>();
  reg<TextComponent>();
}

} // namespace liquid

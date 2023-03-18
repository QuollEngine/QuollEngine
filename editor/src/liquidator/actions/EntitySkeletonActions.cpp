#include "liquid/core/Base.h"
#include "EntitySkeletonActions.h"

namespace liquid::editor {

EntityToggleSkeletonDebugBones::EntityToggleSkeletonDebugBones(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityToggleSkeletonDebugBones::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  if (scene.entityDatabase.has<SkeletonDebug>(mEntity)) {
    scene.entityDatabase.remove<SkeletonDebug>(mEntity);
    return {};
  }

  auto &skeleton = scene.entityDatabase.get<Skeleton>(mEntity);

  SkeletonDebug skeletonDebug{};
  auto numBones = skeleton.numJoints * 2;
  skeletonDebug.bones.reserve(numBones);

  for (uint32_t joint = 0; joint < skeleton.numJoints; ++joint) {
    skeletonDebug.bones.push_back(skeleton.jointParents.at(joint));
    skeletonDebug.bones.push_back(joint);
  }

  skeletonDebug.boneTransforms.resize(numBones, glm::mat4{1.0f});

  scene.entityDatabase.set(mEntity, skeletonDebug);

  return ActionExecutorResult();
}

bool EntityToggleSkeletonDebugBones::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<Skeleton>(mEntity);
}

EntitySetSkeleton::EntitySetSkeleton(Entity entity, SkeletonAssetHandle handle)
    : mEntity(entity), mHandle(handle) {}

ActionExecutorResult EntitySetSkeleton::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  const auto &asset = state.assetRegistry.getSkeletons().getAsset(mHandle).data;

  size_t numJoints = asset.jointLocalPositions.size();

  Skeleton skeleton{};
  skeleton.assetHandle = mHandle;
  skeleton.numJoints = static_cast<uint32_t>(numJoints);
  skeleton.jointNames.resize(numJoints);
  skeleton.jointParents.resize(numJoints);
  skeleton.jointLocalPositions.resize(numJoints);
  skeleton.jointLocalRotations.resize(numJoints);
  skeleton.jointLocalScales.resize(numJoints);
  skeleton.jointInverseBindMatrices.resize(numJoints);
  skeleton.jointWorldTransforms.resize(numJoints, glm::mat4{1.0f});
  skeleton.jointFinalTransforms.resize(numJoints, glm::mat4{1.0f});

  for (size_t i = 0; i < numJoints; ++i) {
    skeleton.jointNames.at(i) = asset.jointNames.at(i);
    skeleton.jointParents.at(i) = asset.jointParents.at(i);
    skeleton.jointLocalPositions.at(i) = asset.jointLocalPositions.at(i);
    skeleton.jointLocalRotations.at(i) = asset.jointLocalRotations.at(i);
    skeleton.jointLocalScales.at(i) = asset.jointLocalScales.at(i);
    skeleton.jointInverseBindMatrices.at(i) =
        asset.jointInverseBindMatrices.at(i);
  }

  scene.entityDatabase.set(mEntity, skeleton);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetSkeleton::predicate(WorkspaceState &state) {
  return state.assetRegistry.getSkeletons().hasAsset(mHandle);
}

} // namespace liquid::editor

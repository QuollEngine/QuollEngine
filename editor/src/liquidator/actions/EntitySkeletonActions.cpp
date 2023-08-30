#include "quoll/core/Base.h"
#include "EntitySkeletonActions.h"

namespace quoll::editor {

EntityToggleSkeletonDebugBones::EntityToggleSkeletonDebugBones(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityToggleSkeletonDebugBones::onExecute(WorkspaceState &state,
                                          AssetRegistry &assetRegistry) {
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

bool EntityToggleSkeletonDebugBones::predicate(WorkspaceState &state,
                                               AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<Skeleton>(mEntity);
}

EntityDeleteSkeleton::EntityDeleteSkeleton(Entity entity) : mEntity(entity) {}

ActionExecutorResult
EntityDeleteSkeleton::onExecute(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldComponent = scene.entityDatabase.get<Skeleton>(mEntity);

  scene.entityDatabase.remove<Skeleton>(mEntity);

  if (scene.entityDatabase.has<SkeletonDebug>(mEntity)) {
    mOldSkeletonDebug = scene.entityDatabase.get<SkeletonDebug>(mEntity);
    scene.entityDatabase.remove<SkeletonDebug>(mEntity);
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntityDeleteSkeleton::onUndo(WorkspaceState &state,
                             AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(mEntity, mOldComponent);
  if (mOldSkeletonDebug.has_value()) {
    scene.entityDatabase.set(mEntity, mOldSkeletonDebug.value());
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntityDeleteSkeleton::predicate(WorkspaceState &state,
                                     AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<Skeleton>(mEntity);
}

} // namespace quoll::editor

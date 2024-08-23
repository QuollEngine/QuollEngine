#include "quoll/core/Base.h"
#include "EntitySkeletonActions.h"

namespace quoll::editor {

EntityToggleSkeletonDebugBones::EntityToggleSkeletonDebugBones(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityToggleSkeletonDebugBones::onExecute(WorkspaceState &state,
                                          AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (mEntity.has<SkeletonDebug>()) {
    mEntity.remove<SkeletonDebug>();
    return {};
  }

  auto skeleton = mEntity.get_ref<Skeleton>();

  SkeletonDebug skeletonDebug{};
  auto numBones = skeleton->numJoints * 2;
  skeletonDebug.bones.reserve(numBones);

  for (u32 joint = 0; joint < skeleton->numJoints; ++joint) {
    skeletonDebug.bones.push_back(skeleton->jointParents.at(joint));
    skeletonDebug.bones.push_back(joint);
  }

  skeletonDebug.boneTransforms.resize(numBones, glm::mat4{1.0f});

  mEntity.set(skeletonDebug);

  return ActionExecutorResult();
}

bool EntityToggleSkeletonDebugBones::predicate(WorkspaceState &state,
                                               AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return mEntity.has<Skeleton>();
}

EntityDeleteSkeleton::EntityDeleteSkeleton(Entity entity) : mEntity(entity) {}

ActionExecutorResult
EntityDeleteSkeleton::onExecute(WorkspaceState &state,
                                AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldComponent = *mEntity.get_ref<Skeleton>().get();

  mEntity.remove<Skeleton>();

  if (mEntity.has<SkeletonDebug>()) {
    mOldSkeletonDebug = *mEntity.get_ref<SkeletonDebug>().get();
    mEntity.remove<SkeletonDebug>();
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntityDeleteSkeleton::onUndo(WorkspaceState &state,
                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.set(mOldComponent);
  if (mOldSkeletonDebug.has_value()) {
    mEntity.set(mOldSkeletonDebug.value());
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntityDeleteSkeleton::predicate(WorkspaceState &state,
                                     AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return mEntity.has<Skeleton>();
}

} // namespace quoll::editor

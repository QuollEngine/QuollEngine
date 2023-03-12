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

} // namespace liquid::editor

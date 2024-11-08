#include "quoll/core/Base.h"
#include "quoll/scene/WorldTransform.h"
#include "EntityTransformActions.h"

namespace quoll::editor {

EntitySetLocalTransformContinuous::EntitySetLocalTransformContinuous(
    Entity entity, std::optional<LocalTransform> oldLocalTransform,
    std::optional<LocalTransform> newLocalTransform)
    : mEntity(entity), mOldLocalTransform(oldLocalTransform),
      mNewLocalTransform(newLocalTransform) {}

void EntitySetLocalTransformContinuous::setNewComponent(
    LocalTransform localTransformFinal) {
  mNewLocalTransform = localTransformFinal;
}

ActionExecutorResult
EntitySetLocalTransformContinuous::onExecute(WorkspaceState &state,
                                             AssetCache &assetCache) {
  auto &scene = state.scene;
  scene.entityDatabase.set(mEntity, mNewLocalTransform.value());
  if (!scene.entityDatabase.has<WorldTransform>(mEntity)) {
    scene.entityDatabase.set<WorldTransform>(mEntity, {});
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntitySetLocalTransformContinuous::onUndo(WorkspaceState &state,
                                          AssetCache &assetCache) {
  auto &scene = state.scene;

  if (mOldLocalTransform.has_value()) {
    scene.entityDatabase.set(mEntity, mOldLocalTransform.value());
    if (!scene.entityDatabase.has<WorldTransform>(mEntity)) {
      scene.entityDatabase.set<WorldTransform>(mEntity, {});
    }
  } else {
    scene.entityDatabase.remove<LocalTransform>(mEntity);
    if (scene.entityDatabase.has<WorldTransform>(mEntity)) {
      scene.entityDatabase.remove<WorldTransform>(mEntity);
    }
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetLocalTransformContinuous::predicate(WorkspaceState &state,
                                                  AssetCache &assetCache) {
  return mNewLocalTransform.has_value();
}

} // namespace quoll::editor

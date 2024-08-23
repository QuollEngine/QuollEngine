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
                                             AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  mEntity.set(mNewLocalTransform.value());
  if (!mEntity.has<WorldTransform>()) {
    mEntity.set<WorldTransform>({});
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntitySetLocalTransformContinuous::onUndo(WorkspaceState &state,
                                          AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  if (mOldLocalTransform.has_value()) {
    mEntity.set(mOldLocalTransform.value());
    if (!mEntity.has<WorldTransform>()) {
      mEntity.set<WorldTransform>({});
    }
  } else {
    mEntity.remove<LocalTransform>();
    if (mEntity.has<WorldTransform>()) {
      mEntity.remove<WorldTransform>();
    }
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetLocalTransformContinuous::predicate(
    WorkspaceState &state, AssetRegistry &assetRegistry) {
  return mNewLocalTransform.has_value();
}

} // namespace quoll::editor

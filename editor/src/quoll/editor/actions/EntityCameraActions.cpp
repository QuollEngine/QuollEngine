#include "quoll/core/Base.h"
#include "quoll/scene/Camera.h"
#include "EntityCameraActions.h"

namespace quoll::editor {

EntityCreatePerspectiveLens::EntityCreatePerspectiveLens(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityCreatePerspectiveLens::onExecute(WorkspaceState &state,
                                       AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.set<PerspectiveLens>({});
  mEntity.add<AutoAspectRatio>();
  mEntity.set<Camera>({});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntityCreatePerspectiveLens::onUndo(WorkspaceState &state,
                                    AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.remove<PerspectiveLens>();
  mEntity.remove<AutoAspectRatio>();
  mEntity.remove<Camera>();

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntityCreatePerspectiveLens::predicate(WorkspaceState &state,
                                            AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return !mEntity.has<PerspectiveLens>();
}

EntityDeletePerspectiveLens::EntityDeletePerspectiveLens(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityDeletePerspectiveLens::onExecute(WorkspaceState &state,
                                       AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mOldPerspectiveLens = *mEntity.get_ref<PerspectiveLens>().get();
  mEntity.remove<PerspectiveLens>();

  if (mEntity.has<AutoAspectRatio>()) {
    mHasAspectRatio = true;
    mEntity.remove<AutoAspectRatio>();
  }

  if (mEntity.has<Camera>()) {
    mEntity.remove<Camera>();
  }

  mIsActiveCamera = state.activeCamera == mEntity;
  if (mIsActiveCamera) {
    state.activeCamera = scene.dummyCamera;
  }

  mIsStartingCamera = scene.activeCamera == mEntity;
  if (mIsStartingCamera) {
    scene.activeCamera = scene.dummyCamera;
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.saveScene = mIsStartingCamera;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntityDeletePerspectiveLens::onUndo(WorkspaceState &state,
                                    AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mEntity.set(mOldPerspectiveLens);

  if (mHasAspectRatio) {
    mEntity.add<AutoAspectRatio>();
  }

  // Camera will be created regardless
  // if it existed before
  mEntity.set<Camera>({});

  if (mIsActiveCamera) {
    state.activeCamera = mEntity;
  }

  if (mIsStartingCamera) {
    scene.activeCamera = mEntity;
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  res.saveScene = mIsStartingCamera;
  return res;
}

bool EntityDeletePerspectiveLens::predicate(WorkspaceState &state,
                                            AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return mEntity.has<PerspectiveLens>();
}

} // namespace quoll::editor

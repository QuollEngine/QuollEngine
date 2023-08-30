#include "quoll/core/Base.h"
#include "EntityCameraActions.h"

namespace quoll::editor {

EntityCreatePerspectiveLens::EntityCreatePerspectiveLens(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityCreatePerspectiveLens::onExecute(WorkspaceState &state,
                                       AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<PerspectiveLens>(mEntity, {});
  scene.entityDatabase.set<AutoAspectRatio>(mEntity, {});
  scene.entityDatabase.set<Camera>(mEntity, {});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
EntityCreatePerspectiveLens::onUndo(WorkspaceState &state,
                                    AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.remove<PerspectiveLens>(mEntity);
  scene.entityDatabase.remove<AutoAspectRatio>(mEntity);
  scene.entityDatabase.remove<Camera>(mEntity);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntityCreatePerspectiveLens::predicate(WorkspaceState &state,
                                            AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return !scene.entityDatabase.has<PerspectiveLens>(mEntity);
}

EntityDeletePerspectiveLens::EntityDeletePerspectiveLens(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityDeletePerspectiveLens::onExecute(WorkspaceState &state,
                                       AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldPerspectiveLens = scene.entityDatabase.get<PerspectiveLens>(mEntity);
  scene.entityDatabase.remove<PerspectiveLens>(mEntity);

  if (scene.entityDatabase.has<AutoAspectRatio>(mEntity)) {
    mOldAspectRatio = scene.entityDatabase.get<AutoAspectRatio>(mEntity);
    scene.entityDatabase.remove<AutoAspectRatio>(mEntity);
  }

  if (scene.entityDatabase.has<Camera>(mEntity)) {
    scene.entityDatabase.remove<Camera>(mEntity);
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
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(mEntity, mOldPerspectiveLens);

  if (mOldAspectRatio.has_value()) {
    scene.entityDatabase.set(mEntity, mOldAspectRatio.value());
  }

  // Camera will be created regardless
  // if it existed before
  scene.entityDatabase.set<Camera>(mEntity, {});

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
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<PerspectiveLens>(mEntity);
}

} // namespace quoll::editor

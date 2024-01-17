#include "quoll/core/Base.h"
#include "quoll/scene/PerspectiveLens.h"
#include "SceneActions.h"

namespace quoll::editor {

SceneSetStartingCamera::SceneSetStartingCamera(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
SceneSetStartingCamera::onExecute(WorkspaceState &state,
                                  AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mPreviousCamera = scene.activeCamera;
  scene.activeCamera = mEntity;

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
SceneSetStartingCamera::onUndo(WorkspaceState &state,
                               AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  scene.activeCamera = mPreviousCamera;

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneSetStartingCamera::predicate(WorkspaceState &state,
                                       AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return scene.activeCamera != mEntity &&
         scene.entityDatabase.has<PerspectiveLens>(mEntity);
}

SceneSetStartingEnvironment::SceneSetStartingEnvironment(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
SceneSetStartingEnvironment::onExecute(WorkspaceState &state,
                                       AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mPreviousEnvironment = scene.activeEnvironment;
  scene.activeEnvironment = mEntity;

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
SceneSetStartingEnvironment::onUndo(WorkspaceState &state,
                                    AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  scene.activeEnvironment = mPreviousEnvironment;

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneSetStartingEnvironment::predicate(WorkspaceState &state,
                                            AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return scene.activeEnvironment != mEntity;
}

ActionExecutorResult
SceneRemoveStartingEnvironment::onExecute(WorkspaceState &state,
                                          AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  mPreviousEnvironment = scene.activeEnvironment;
  scene.activeEnvironment = scene.dummyEnvironment;

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
SceneRemoveStartingEnvironment::onUndo(WorkspaceState &state,
                                       AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  scene.activeEnvironment = mPreviousEnvironment;

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneRemoveStartingEnvironment::predicate(WorkspaceState &state,
                                               AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return scene.activeEnvironment != scene.dummyEnvironment;
}

} // namespace quoll::editor

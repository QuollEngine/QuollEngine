#include "liquid/core/Base.h"
#include "SceneActions.h"

namespace liquid::editor {

SceneSetStartingCamera::SceneSetStartingCamera(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult SceneSetStartingCamera::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mPreviousCamera = scene.activeCamera;
  scene.activeCamera = mEntity;

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult SceneSetStartingCamera::onUndo(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.activeCamera = mPreviousCamera;

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneSetStartingCamera::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.activeCamera != mEntity &&
         scene.entityDatabase.has<PerspectiveLens>(mEntity);
}

ActionExecutorResult SceneRemoveSkybox::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldSkybox = scene.entityDatabase.get<EnvironmentSkybox>(scene.environment);

  scene.entityDatabase.remove<EnvironmentSkybox>(scene.environment);

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult SceneRemoveSkybox::onUndo(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(scene.environment, mOldSkybox);

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

bool SceneRemoveSkybox::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<EnvironmentSkybox>(scene.environment);
}

SceneChangeSkyboxType::SceneChangeSkyboxType(EnvironmentSkyboxType type)
    : mType(type) {}

ActionExecutorResult SceneChangeSkyboxType::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  if (scene.entityDatabase.has<EnvironmentSkybox>(scene.environment)) {
    mOldSkybox = scene.entityDatabase.get<EnvironmentSkybox>(scene.environment);
  }

  EnvironmentSkybox skybox{mType};
  scene.entityDatabase.set(scene.environment, skybox);

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult SceneChangeSkyboxType::onUndo(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  if (mOldSkybox.has_value()) {
    scene.entityDatabase.set(scene.environment, mOldSkybox.value());
  } else {
    scene.entityDatabase.remove<EnvironmentSkybox>(scene.environment);
  }

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneChangeSkyboxType::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  // If skybox does not exist, the action
  // can be executed
  if (!scene.entityDatabase.has<EnvironmentSkybox>(scene.environment)) {
    return true;
  }

  const auto &skybox =
      scene.entityDatabase.get<EnvironmentSkybox>(scene.environment);

  return skybox.type != mType;
}

SceneSetSkyboxColor::SceneSetSkyboxColor(glm::vec4 color) : mColor(color) {}

ActionExecutorResult SceneSetSkyboxColor::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldSkybox = scene.entityDatabase.get<EnvironmentSkybox>(scene.environment);

  scene.entityDatabase.set<EnvironmentSkybox>(
      scene.environment, {EnvironmentSkyboxType::Color, {}, mColor});

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult SceneSetSkyboxColor::onUndo(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(scene.environment, mOldSkybox);

  ActionExecutorResult res{};
  res.saveScene = true;

  return res;
}

bool SceneSetSkyboxColor::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  if (!scene.entityDatabase.has<EnvironmentSkybox>(scene.environment)) {
    return false;
  }

  return scene.entityDatabase.get<EnvironmentSkybox>(scene.environment).type ==
         EnvironmentSkyboxType::Color;
}

SceneSetSkyboxTexture::SceneSetSkyboxTexture(
    liquid::EnvironmentAssetHandle texture)
    : mTexture(texture) {}

ActionExecutorResult SceneSetSkyboxTexture::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldSkybox = scene.entityDatabase.get<EnvironmentSkybox>(scene.environment);

  scene.entityDatabase.set<EnvironmentSkybox>(
      scene.environment, {EnvironmentSkyboxType::Texture, mTexture});

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult SceneSetSkyboxTexture::onUndo(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(scene.environment, mOldSkybox);

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

bool SceneSetSkyboxTexture::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  if (!scene.entityDatabase.has<EnvironmentSkybox>(scene.environment)) {
    return false;
  }

  return scene.entityDatabase.get<EnvironmentSkybox>(scene.environment).type ==
         EnvironmentSkyboxType::Texture;
}

ActionExecutorResult SceneRemoveLighting::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  mOldLightingSource =
      scene.entityDatabase.get<EnvironmentLightingSkyboxSource>(
          scene.environment);

  scene.entityDatabase.remove<EnvironmentLightingSkyboxSource>(
      scene.environment);

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult SceneRemoveLighting::onUndo(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<EnvironmentLightingSkyboxSource>(scene.environment,
                                                            mOldLightingSource);

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneRemoveLighting::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<EnvironmentLightingSkyboxSource>(
      scene.environment);
}

ActionExecutorResult
SceneSetSkyboxLightingSource::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<EnvironmentLightingSkyboxSource>(scene.environment,
                                                            {});

  ActionExecutorResult res{};
  res.saveScene = true;
  res.addToHistory = true;
  return res;
}

ActionExecutorResult
SceneSetSkyboxLightingSource::onUndo(WorkspaceState &state) {

  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.remove<EnvironmentLightingSkyboxSource>(
      scene.environment);

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneSetSkyboxLightingSource::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return !scene.entityDatabase.has<EnvironmentLightingSkyboxSource>(
      scene.environment);
}

} // namespace liquid::editor

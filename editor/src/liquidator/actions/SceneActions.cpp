#include "liquid/core/Base.h"
#include "SceneActions.h"

namespace liquid::editor {

SceneSetStartingCamera::SceneSetStartingCamera(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult SceneSetStartingCamera::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.activeCamera = mEntity;

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

  scene.entityDatabase.remove<EnvironmentSkybox>(scene.environment);

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneRemoveSkybox::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<EnvironmentSkybox>(scene.environment);
}

SceneSetSkyboxColor::SceneSetSkyboxColor(glm::vec4 color) : mColor(color) {}

ActionExecutorResult SceneSetSkyboxColor::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<EnvironmentSkybox>(
      scene.environment, {EnvironmentSkyboxType::Color, {}, mColor});

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneSetSkyboxColor::predicate(WorkspaceState &state) { return true; }

SceneSetSkyboxTexture::SceneSetSkyboxTexture(
    liquid::EnvironmentAssetHandle texture)
    : mTexture(texture) {}

ActionExecutorResult SceneSetSkyboxTexture::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<EnvironmentSkybox>(
      scene.environment, {EnvironmentSkyboxType::Texture, mTexture});

  ActionExecutorResult res{};
  res.saveScene = true;
  return res;
}

bool SceneSetSkyboxTexture::predicate(WorkspaceState &state) { return true; }

ActionExecutorResult SceneRemoveLighting::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.remove<EnvironmentLightingSkyboxSource>(
      scene.environment);

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
  return res;
}

bool SceneSetSkyboxLightingSource::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return !scene.entityDatabase.has<EnvironmentLightingSkyboxSource>(
      scene.environment);
}

} // namespace liquid::editor

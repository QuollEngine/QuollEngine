#include "liquid/core/Base.h"
#include "DeleteEntityAction.h"
#include "EntityLightActions.h"

namespace liquid::editor {

EntitySetDirectionalLight::EntitySetDirectionalLight(Entity entity,
                                                     DirectionalLight light)
    : mEntity(entity), mLight(light) {}

ActionExecutorResult
EntitySetDirectionalLight::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  if (scene.entityDatabase.has<PointLight>(mEntity)) {
    scene.entityDatabase.remove<PointLight>(mEntity);
  }

  scene.entityDatabase.set(mEntity, mLight);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetDirectionalLight::predicate(WorkspaceState &state) {
  return true;
}

EntityEnableCascadedShadowMap::EntityEnableCascadedShadowMap(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityEnableCascadedShadowMap::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<CascadedShadowMap>(mEntity, {});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntityEnableCascadedShadowMap::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return !scene.entityDatabase.has<CascadedShadowMap>(mEntity);
}

EntityDisableCascadedShadowMap::EntityDisableCascadedShadowMap(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntityDisableCascadedShadowMap::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.remove<CascadedShadowMap>(mEntity);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntityDisableCascadedShadowMap::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<CascadedShadowMap>(mEntity);
}

EntitySetCascadedShadowMap::EntitySetCascadedShadowMap(
    Entity entity, CascadedShadowMap shadowMap)
    : mEntity(entity), mShadowMap(shadowMap) {}

ActionExecutorResult
EntitySetCascadedShadowMap::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(mEntity, mShadowMap);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetCascadedShadowMap::predicate(WorkspaceState &state) {
  return true;
}

EntitySetPointLight::EntitySetPointLight(Entity entity, PointLight light)
    : mEntity(entity), mLight(light) {}

ActionExecutorResult EntitySetPointLight::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  if (scene.entityDatabase.has<DirectionalLight>(mEntity)) {
    scene.entityDatabase.remove<DirectionalLight>(mEntity);
  }

  scene.entityDatabase.set(mEntity, mLight);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetPointLight::predicate(WorkspaceState &state) { return true; }

} // namespace liquid::editor

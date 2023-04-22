#include "liquid/core/Base.h"
#include "EntityCameraActions.h"

namespace liquid::editor {

EntitySetPerspectiveLens::EntitySetPerspectiveLens(Entity entity,
                                                   PerspectiveLens lens)
    : mEntity(entity), mLens(lens) {}

ActionExecutorResult
EntitySetPerspectiveLens::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set(mEntity, mLens);
  if (!scene.entityDatabase.has<Camera>(mEntity)) {
    scene.entityDatabase.set(mEntity, Camera{});
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetPerspectiveLens::predicate(WorkspaceState &state) { return true; }

EntitySetCameraAutoAspectRatio::EntitySetCameraAutoAspectRatio(Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntitySetCameraAutoAspectRatio::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.set<AutoAspectRatio>(mEntity, {});

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetCameraAutoAspectRatio::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return !scene.entityDatabase.has<AutoAspectRatio>(mEntity);
}

EntitySetCameraCustomAspectRatio::EntitySetCameraCustomAspectRatio(
    Entity entity)
    : mEntity(entity) {}

ActionExecutorResult
EntitySetCameraCustomAspectRatio::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.remove<AutoAspectRatio>(mEntity);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  return res;
}

bool EntitySetCameraCustomAspectRatio::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<AutoAspectRatio>(mEntity);
}

EntityDeleteCamera::EntityDeleteCamera(Entity entity) : mEntity(entity) {}

ActionExecutorResult EntityDeleteCamera::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  scene.entityDatabase.remove<PerspectiveLens>(mEntity);

  if (scene.entityDatabase.has<AutoAspectRatio>(mEntity)) {
    scene.entityDatabase.remove<AutoAspectRatio>(mEntity);
  }

  if (scene.entityDatabase.has<Camera>(mEntity)) {
    scene.entityDatabase.remove<Camera>(mEntity);
  }

  if (state.activeCamera == mEntity) {
    state.activeCamera = scene.dummyCamera;
  }

  bool isStartingCamera = scene.activeCamera == mEntity;
  if (isStartingCamera) {
    scene.activeCamera = scene.dummyCamera;
  }

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mEntity);
  res.saveScene = isStartingCamera;
  return res;
}

bool EntityDeleteCamera::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<PerspectiveLens>(mEntity);
}

} // namespace liquid::editor

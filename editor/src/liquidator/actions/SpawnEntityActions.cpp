#include "liquid/core/Base.h"
#include "liquid/entity/EntitySpawner.h"

#include "EntityMeshActions.h"
#include "EntitySkeletonActions.h"
#include "EntityTransformActions.h"
#include "EntityLightActions.h"
#include "EntityNameActions.h"

#include "SpawnEntityActions.h"

namespace liquid::editor {

/**
 * @brief Get transform from camera view matrix
 *
 * @param viewMatrix Camera view matrix
 * @return Local transform
 */
static LocalTransform getTransformFromView(glm::mat4 viewMatrix) {
  static constexpr glm::vec3 DistanceFromEye{0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(DistanceFromEye);

  LocalTransform transform{};
  transform.localPosition = orientation[3];

  return transform;
}

/**
 * @brief Check if prefab is valid
 *
 * @param assetRegistry Asset registry
 * @param handle Prefab asset handle
 * @retval true Prefab exists and is not empty
 * @retval false Prefab does not exist or is empty
 */
static bool isPrefabValid(AssetRegistry &assetRegistry,
                          PrefabAssetHandle handle) {
  const auto &prefabs = assetRegistry.getPrefabs();
  if (!prefabs.hasAsset(handle)) {
    return false;
  }

  const auto &prefab = prefabs.getAsset(handle).data;

  return !prefab.animators.empty() || !prefab.meshes.empty() ||
         !prefab.skeletons.empty() || !prefab.skinnedMeshes.empty() ||
         !prefab.transforms.empty();
}

ActionExecutorResult SpawnEmptyEntityAtView::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  const auto &viewMatrix =
      scene.entityDatabase.get<Camera>(state.camera).viewMatrix;

  auto transform = getTransformFromView(viewMatrix);

  auto entity = EntitySpawner(scene.entityDatabase, state.assetRegistry)
                    .spawnEmpty(transform);

  EntitySetName(entity, {"New entity"}).onExecute(state);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(entity);
  return res;
}

bool SpawnEmptyEntityAtView::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<Camera>(state.camera);
}

SpawnPrefabAtTransform::SpawnPrefabAtTransform(PrefabAssetHandle handle,
                                               LocalTransform transform)
    : mHandle(handle), mTransform(transform) {}

ActionExecutorResult SpawnPrefabAtTransform::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  ActionExecutorResult res{};
  res.entitiesToSave = EntitySpawner(scene.entityDatabase, state.assetRegistry)
                           .spawnPrefab(mHandle, mTransform);
  return res;
}

bool SpawnPrefabAtTransform::predicate(WorkspaceState &state) {
  return isPrefabValid(state.assetRegistry, mHandle);
}

SpawnPrefabAtView::SpawnPrefabAtView(PrefabAssetHandle handle, Entity camera)
    : mHandle(handle), mCamera(camera) {}

ActionExecutorResult SpawnPrefabAtView::onExecute(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  const auto &viewMatrix = scene.entityDatabase.get<Camera>(mCamera).viewMatrix;

  return SpawnPrefabAtTransform(mHandle, getTransformFromView(viewMatrix))
      .onExecute(state);
}

bool SpawnPrefabAtView::predicate(WorkspaceState &state) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return isPrefabValid(state.assetRegistry, mHandle) &&
         scene.entityDatabase.has<Camera>(mCamera);
}

} // namespace liquid::editor

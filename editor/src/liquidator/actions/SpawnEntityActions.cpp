#include "liquid/core/Base.h"

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

  auto entity = scene.entityDatabase.create();

  auto transform = getTransformFromView(viewMatrix);
  EntitySetLocalTransform(entity, transform).onExecute(state);
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

  const auto &asset = state.assetRegistry.getPrefabs().getAsset(mHandle).data;

  std::unordered_map<uint32_t, size_t> entityMap;
  std::vector<Entity> entities;

  auto getOrCreateEntity = [&entityMap, &entities, &scene,
                            &state](uint32_t localId) mutable {
    if (entityMap.find(localId) == entityMap.end()) {
      auto entity = scene.entityDatabase.create();
      EntitySetLocalTransform(entity, {}).onExecute(state);
      entities.push_back(entity);
      entityMap.insert_or_assign(localId, entities.size() - 1);
      return entity;
    }

    return entities.at(entityMap.at(localId));
  };

  for (const auto &pTransform : asset.transforms) {
    if (pTransform.value.parent >= 0) {
      auto parent = getOrCreateEntity(pTransform.value.parent);
      auto entity = getOrCreateEntity(pTransform.entity);
      scene.entityDatabase.set(entity, Parent{parent});

      if (!scene.entityDatabase.has<Children>(entity)) {
        scene.entityDatabase.set<Children>(parent, {{entity}});
      } else {
        scene.entityDatabase.get<Children>(parent).children.push_back(entity);
      }
    }
  }

  for (const auto &pTransform : asset.transforms) {
    auto entity = getOrCreateEntity(pTransform.entity);
    LocalTransform transform{};
    transform.localPosition = pTransform.value.position;
    transform.localRotation = pTransform.value.rotation;
    transform.localScale = pTransform.value.scale;

    EntitySetLocalTransform(entity, transform).onExecute(state);
  }

  for (const auto &pMesh : asset.meshes) {
    auto entity = getOrCreateEntity(pMesh.entity);
    EntitySetMesh(entity, pMesh.value).onExecute(state);
  }

  for (const auto &pSkinnedMesh : asset.skinnedMeshes) {
    auto entity = getOrCreateEntity(pSkinnedMesh.entity);
    EntitySetSkinnedMesh(entity, pSkinnedMesh.value).onExecute(state);
  }

  for (const auto &pSkeleton : asset.skeletons) {
    auto entity = getOrCreateEntity(pSkeleton.entity);
    EntitySetSkeleton(entity, pSkeleton.value).onExecute(state);
  }

  for (auto &item : asset.animators) {
    auto entity = getOrCreateEntity(item.entity);
    scene.entityDatabase.set(entity, item.value);
  }

  for (auto &item : asset.directionalLights) {
    auto entity = getOrCreateEntity(item.entity);
    EntitySetDirectionalLight(entity, item.value).onExecute(state);
  }

  for (auto &item : asset.pointLights) {
    auto entity = getOrCreateEntity(item.entity);
    EntitySetPointLight(entity, item.value).onExecute(state);
  }

  std::vector<Entity> rootEntities;
  for (auto entity : entities) {
    if (!scene.entityDatabase.has<Parent>(entity)) {
      rootEntities.push_back(entity);
    }
  }

  LIQUID_ASSERT(!rootEntities.empty(), "No entities found in prefab");

  auto rootNode = Entity::Null;
  // If more than one root exists,
  // create root node
  if (rootEntities.size() > 1) {
    rootNode = scene.entityDatabase.create();
    scene.entityDatabase.set<Children>(rootNode, {rootEntities});
    for (auto entity : rootEntities) {
      scene.entityDatabase.set<Parent>(entity, {rootNode});
    }
    entities.push_back(rootNode);
  } else {
    rootNode = rootEntities.at(0);
  }

  EntitySetLocalTransform(rootNode, mTransform).onExecute(state);

  ActionExecutorResult res{};
  res.entitiesToSave = entities;

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

#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"

#include "EntityManager.h"

namespace liquid::editor {

EntityManager::EntityManager(AssetManager &assetManager, Renderer &renderer,
                             const std::filesystem::path &scenePath)
    : mScenePath(scenePath), mRenderer(renderer), mAssetManager(assetManager),
      mSceneIO(mAssetManager.getAssetRegistry(), mScene) {}

void EntityManager::save(Entity entity) {
  if (mInSimulation)
    return;

  mSceneIO.saveEntity(entity, mScenePath / "main.lqscene");
}

Entity EntityManager::createEmptyEntity(Entity parent,
                                        const LocalTransform &transform,
                                        const String &name) {

  auto &entityDatabase = getActiveEntityDatabase();

  auto entity = entityDatabase.create();
  entityDatabase.set(entity, transform);
  entityDatabase.set<WorldTransform>(entity, {});

  if (entityDatabase.exists(parent)) {
    entityDatabase.set<Parent>(entity, {parent});

    if (!entityDatabase.has<Children>(parent)) {
      entityDatabase.set<Children>(parent, {});
    }

    entityDatabase.get<Children>(parent).children.push_back(entity);
  }

  setName(entity, name);

  return entity;
}

Entity EntityManager::createEmptyEntity(EditorCamera &camera, Entity parent,
                                        const String &name, bool saveToFile) {
  auto entity = createEmptyEntity(parent, getTransformFromCamera(camera), name);

  if (saveToFile) {
    save(entity);
  }

  return entity;
}

bool EntityManager::loadScene() {
  mSceneIO.loadScene(mScenePath / "main.lqscene");

  return true;
}

void EntityManager::setSkeletonForEntity(Entity entity,
                                         SkeletonAssetHandle handle) {
  const auto &skeleton =
      mAssetManager.getAssetRegistry().getSkeletons().getAsset(handle).data;

  Skeleton skeletonInstance{};
  skeletonInstance.jointLocalPositions = skeleton.jointLocalPositions;
  skeletonInstance.jointLocalRotations = skeleton.jointLocalRotations;
  skeletonInstance.jointLocalScales = skeleton.jointLocalScales;
  skeletonInstance.jointParents = skeleton.jointParents;
  skeletonInstance.jointInverseBindMatrices = skeleton.jointInverseBindMatrices;
  skeletonInstance.jointNames = skeleton.jointNames;
  skeletonInstance.assetHandle = handle;
  skeletonInstance.numJoints =
      static_cast<uint32_t>(skeleton.jointLocalPositions.size());
  skeletonInstance.jointFinalTransforms.resize(skeletonInstance.numJoints,
                                               glm::mat4{1.0f});
  skeletonInstance.jointWorldTransforms.resize(skeletonInstance.numJoints,
                                               glm::mat4{1.0f});

  getActiveEntityDatabase().set(entity, skeletonInstance);
}

void EntityManager::toggleSkeletonDebugForEntity(Entity entity) {
  auto &entityDatabase = getActiveEntityDatabase();
  if (!entityDatabase.has<Skeleton>(entity)) {
    return;
  }

  if (entityDatabase.has<SkeletonDebug>(entity)) {
    entityDatabase.remove<SkeletonDebug>(entity);
    return;
  }

  auto &skeleton = entityDatabase.get<Skeleton>(entity);

  SkeletonDebug skeletonDebug{};
  auto numBones = skeleton.numJoints * 2;
  skeletonDebug.bones.reserve(numBones);

  for (uint32_t joint = 0; joint < skeleton.numJoints; ++joint) {
    skeletonDebug.bones.push_back(skeleton.jointParents.at(joint));
    skeletonDebug.bones.push_back(joint);
  }

  skeletonDebug.boneTransforms.resize(numBones, glm::mat4{1.0f});

  entityDatabase.set(entity, skeletonDebug);
}

void EntityManager::toggleShadowsForLightEntity(Entity entity) {
  auto &entityDatabase = getActiveEntityDatabase();
  if (entityDatabase.has<CascadedShadowMap>(entity)) {
    entityDatabase.remove<CascadedShadowMap>(entity);
  } else {
    entityDatabase.set<CascadedShadowMap>(entity, {});
  }
}

void EntityManager::setMesh(Entity entity, MeshAssetHandle handle) {
  if (getActiveEntityDatabase().has<SkinnedMesh>(entity)) {
    getActiveEntityDatabase().remove<SkinnedMesh>(entity);
  }

  getActiveEntityDatabase().set<Mesh>(entity, {handle});
}

void EntityManager::setSkinnedMesh(Entity entity,
                                   SkinnedMeshAssetHandle handle) {
  if (getActiveEntityDatabase().has<Mesh>(entity)) {
    getActiveEntityDatabase().remove<Mesh>(entity);
  }
  getActiveEntityDatabase().set<SkinnedMesh>(entity, {handle});
}

void EntityManager::setName(Entity entity, const String &name) {
  if (name.empty()) {
    return;
  }

  getActiveEntityDatabase().set<Name>(entity, {name});
}

void EntityManager::setCamera(Entity entity, const PerspectiveLens &lens,
                              bool autoRatio) {
  getActiveEntityDatabase().set<Camera>(entity, {});
  getActiveEntityDatabase().set<PerspectiveLens>(entity, lens);
  if (autoRatio) {
    getActiveEntityDatabase().set<AutoAspectRatio>(entity, {});
  }
}

void EntityManager::setAudio(Entity entity, AudioAssetHandle source) {
  getActiveEntityDatabase().set<AudioSource>(entity, {source});
}

void EntityManager::setText(Entity entity, Text text) {
  getActiveEntityDatabase().set(entity, text);
}

void EntityManager::setScript(Entity entity, LuaScriptAssetHandle handle) {
  Script script{};
  script.handle = handle;
  getActiveEntityDatabase().set(entity, script);
}

void EntityManager::deleteEntity(Entity entity) {
  if (!mInSimulation) {
    mSceneIO.deleteEntityFilesAndRelations(entity, mScenePath / "main.lqscene");
  }

  getActiveEntityDatabase().set<Delete>(entity, {});
}

void EntityManager::updateLocalTransformUsingWorld(
    Entity entity, const glm::mat4 &worldTransform) {
  auto &entityDatabase = getActiveEntityDatabase();

  glm::vec3 worldPosition;
  glm::quat worldRotation;
  glm::vec3 worldScale;

  glm::vec3 noopSkew;
  glm::vec4 noopPerspective;

  glm::decompose(worldTransform, worldScale, worldRotation, worldPosition,
                 noopSkew, noopPerspective);

  auto &transform = entityDatabase.get<LocalTransform>(entity);

  if (entityDatabase.has<Parent>(entity)) {
    const auto parent = entityDatabase.get<Parent>(entity).parent;
    if (entityDatabase.exists(parent) &&
        entityDatabase.has<WorldTransform>(parent)) {
      const auto &parentWorld =
          entityDatabase.get<WorldTransform>(parent).worldTransform;

      glm::vec3 parentPosition;
      glm::quat parentRotation;
      glm::vec3 parentScale;

      glm::decompose(parentWorld, parentScale, parentRotation, parentPosition,
                     noopSkew, noopPerspective);

      transform.localPosition = worldPosition - parentPosition;
      transform.localScale =
          glm::vec3(worldScale.x / parentScale.x, worldScale.y / parentScale.y,
                    worldScale.z / parentScale.z);
      transform.localRotation =
          glm::toQuat(glm::inverse(parentWorld) * glm::toMat4(worldRotation));
    }
  } else {
    transform.localPosition = worldPosition;
    transform.localScale = worldScale;
    transform.localRotation = worldRotation;
  }

  save(entity);
}

Entity EntityManager::spawnEntity(EditorCamera &camera, Entity root,
                                  uint32_t handle, AssetType type,
                                  bool saveToFile) {
  if (type != AssetType::Prefab) {
    return EntityNull;
  }

  auto &registry = mAssetManager.getAssetRegistry();

  auto &asset =
      registry.getPrefabs().getAsset(static_cast<PrefabAssetHandle>(handle));
  auto parent = createEmptyEntity(camera, root, asset.name, saveToFile);

  uint32_t childIndex = 1;
  std::unordered_map<uint32_t, Entity> entityMap;

  auto getOrCreateEntity = [&entityMap, this, parent, &camera, &childIndex](
                               uint32_t localId,
                               const LocalTransform &transform = {}) mutable {
    if (entityMap.find(localId) == entityMap.end()) {
      auto entity = createEmptyEntity(parent, transform,
                                      "Untitled " + std::to_string(childIndex));
      entityMap.insert_or_assign(localId, entity);
      childIndex++;
    }

    return entityMap.at(localId);
  };

  for (auto &item : asset.data.transforms) {
    LocalTransform transform{};
    transform.localPosition = item.value.position;
    transform.localRotation = item.value.rotation;
    transform.localScale = item.value.scale;

    getOrCreateEntity(item.entity, transform);
  }

  for (auto &item : asset.data.meshes) {
    if (!registry.getMeshes().hasAsset(item.value)) {
      continue;
    }

    setMesh(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.skinnedMeshes) {
    if (!registry.getSkinnedMeshes().hasAsset(item.value)) {
      continue;
    }

    setSkinnedMesh(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.skeletons) {
    if (!registry.getSkeletons().hasAsset(item.value)) {
      continue;
    }

    setSkeletonForEntity(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.animators) {
    auto entity = getOrCreateEntity(item.entity);
    getActiveEntityDatabase().set(entity, item.value);
  }

  if (saveToFile) {
    for (auto [_, entity] : entityMap) {
      save(entity);
    }
  }

  return parent;
}

void EntityManager::useSimulationDatabase() {
  mSimulationScene.entityDatabase.destroy();
  mScene.entityDatabase.duplicate(mSimulationScene.entityDatabase);
  mSimulationScene.activeCamera = mScene.activeCamera;
  mSimulationScene.dummyCamera = mScene.dummyCamera;

  mInSimulation = true;
}

void EntityManager::useEditingDatabase() { mInSimulation = false; }

Entity EntityManager::getStartingCamera() {
  return getActiveScene().activeCamera;
}

Entity EntityManager::getActiveSimulationCamera() {
  return mSimulationScene.activeCamera;
}

void EntityManager::setStartingCamera(Entity camera) {
  getActiveScene().activeCamera = camera;
  mSceneIO.saveStartingCamera(camera, mScenePath / "main.lqscene");
}

void EntityManager::saveEnvironment() {
  mSceneIO.saveEnvironment(mScenePath / "main.lqscene");
}

LocalTransform
EntityManager::getTransformFromCamera(EditorCamera &camera) const {
  auto &scene = mInSimulation ? mSimulationScene : mScene;
  const auto &viewMatrix =
      scene.entityDatabase.get<Camera>(camera.getCamera()).viewMatrix;

  static constexpr glm::vec3 DistanceFromEye{0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(DistanceFromEye);

  LocalTransform transform;
  transform.localPosition = orientation[3];
  return transform;
}

} // namespace liquid::editor

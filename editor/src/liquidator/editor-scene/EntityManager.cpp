#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"

#include "EntityManager.h"

namespace liquidator {

EntityManager::EntityManager(AssetManager &assetManager,
                             liquid::Renderer &renderer,
                             const std::filesystem::path &scenePath)
    : mScenePath(scenePath), mRenderer(renderer), mAssetManager(assetManager),
      mSceneIO(mAssetManager.getAssetRegistry(), mScene) {}

void EntityManager::save(liquid::Entity entity) {
  if (mInSimulation)
    return;

  mSceneIO.saveEntity(entity, mScenePath / "main.lqscene");
}

liquid::Entity
EntityManager::createEmptyEntity(liquid::Entity parent,
                                 const liquid::LocalTransform &transform,
                                 const liquid::String &name) {

  auto &entityDatabase = getActiveEntityDatabase();

  auto entity = entityDatabase.create();
  entityDatabase.set(entity, transform);
  entityDatabase.set<liquid::WorldTransform>(entity, {});

  if (entityDatabase.exists(parent)) {
    entityDatabase.set<liquid::Parent>(entity, {parent});

    if (!entityDatabase.has<liquid::Children>(parent)) {
      entityDatabase.set<liquid::Children>(parent, {});
    }

    entityDatabase.get<liquid::Children>(parent).children.push_back(entity);
  }

  setName(entity, name);

  return entity;
}

liquid::Entity EntityManager::createEmptyEntity(EditorCamera &camera,
                                                liquid::Entity parent,
                                                const liquid::String &name,
                                                bool saveToFile) {
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

void EntityManager::setSkeletonForEntity(liquid::Entity entity,
                                         liquid::SkeletonAssetHandle handle) {
  const auto &skeleton =
      mAssetManager.getAssetRegistry().getSkeletons().getAsset(handle).data;

  liquid::Skeleton skeletonInstance{};
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

void EntityManager::toggleSkeletonDebugForEntity(liquid::Entity entity) {
  auto &entityDatabase = getActiveEntityDatabase();
  if (!entityDatabase.has<liquid::Skeleton>(entity)) {
    return;
  }

  if (entityDatabase.has<liquid::SkeletonDebug>(entity)) {
    entityDatabase.remove<liquid::SkeletonDebug>(entity);
    return;
  }

  auto &skeleton = entityDatabase.get<liquid::Skeleton>(entity);

  liquid::SkeletonDebug skeletonDebug{};
  auto numBones = skeleton.numJoints * 2;
  skeletonDebug.bones.reserve(numBones);

  for (uint32_t joint = 0; joint < skeleton.numJoints; ++joint) {
    skeletonDebug.bones.push_back(skeleton.jointParents.at(joint));
    skeletonDebug.bones.push_back(joint);
  }

  skeletonDebug.boneTransforms.resize(numBones, glm::mat4{1.0f});

  entityDatabase.set(entity, skeletonDebug);
}

void EntityManager::setMesh(liquid::Entity entity,
                            liquid::MeshAssetHandle handle) {
  if (getActiveEntityDatabase().has<liquid::SkinnedMesh>(entity)) {
    getActiveEntityDatabase().remove<liquid::SkinnedMesh>(entity);
  }

  getActiveEntityDatabase().set<liquid::Mesh>(entity, {handle});
}

void EntityManager::setSkinnedMesh(liquid::Entity entity,
                                   liquid::SkinnedMeshAssetHandle handle) {
  if (getActiveEntityDatabase().has<liquid::Mesh>(entity)) {
    getActiveEntityDatabase().remove<liquid::Mesh>(entity);
  }
  getActiveEntityDatabase().set<liquid::SkinnedMesh>(entity, {handle});
}

void EntityManager::setName(liquid::Entity entity, const liquid::String &name) {
  if (name.empty()) {
    return;
  }

  getActiveEntityDatabase().set<liquid::Name>(entity, {name});
}

void EntityManager::setCamera(liquid::Entity entity,
                              const liquid::PerspectiveLens &lens,
                              bool autoRatio) {
  getActiveEntityDatabase().set<liquid::Camera>(entity, {});
  getActiveEntityDatabase().set<liquid::PerspectiveLens>(entity, lens);
  if (autoRatio) {
    getActiveEntityDatabase().set<liquid::AutoAspectRatio>(entity, {});
  }
}

void EntityManager::setAudio(liquid::Entity entity,
                             liquid::AudioAssetHandle source) {
  getActiveEntityDatabase().set<liquid::AudioSource>(entity, {source});
}

void EntityManager::setText(liquid::Entity entity, liquid::Text text) {
  getActiveEntityDatabase().set(entity, text);
}

void EntityManager::setScript(liquid::Entity entity,
                              liquid::LuaScriptAssetHandle handle) {
  liquid::Script script{};
  script.handle = handle;
  getActiveEntityDatabase().set(entity, script);
}

void EntityManager::deleteEntity(liquid::Entity entity) {
  if (!mInSimulation) {
    mSceneIO.deleteEntityFilesAndRelations(entity, mScenePath / "main.lqscene");
  }

  getActiveEntityDatabase().set<liquid::Delete>(entity, {});
}

void EntityManager::updateLocalTransformUsingWorld(
    liquid::Entity entity, const glm::mat4 &worldTransform) {
  auto &entityDatabase = getActiveEntityDatabase();

  glm::vec3 worldPosition;
  glm::quat worldRotation;
  glm::vec3 worldScale;

  glm::vec3 noopSkew;
  glm::vec4 noopPerspective;

  glm::decompose(worldTransform, worldScale, worldRotation, worldPosition,
                 noopSkew, noopPerspective);

  auto &transform = entityDatabase.get<liquid::LocalTransform>(entity);

  if (entityDatabase.has<liquid::Parent>(entity)) {
    const auto parent = entityDatabase.get<liquid::Parent>(entity).parent;
    if (entityDatabase.exists(parent) &&
        entityDatabase.has<liquid::WorldTransform>(parent)) {
      const auto &parentWorld =
          entityDatabase.get<liquid::WorldTransform>(parent).worldTransform;

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

liquid::Entity EntityManager::spawnEntity(EditorCamera &camera,
                                          liquid::Entity root, uint32_t handle,
                                          liquid::AssetType type,
                                          bool saveToFile) {
  if (type != liquid::AssetType::Prefab) {
    return liquid::EntityNull;
  }

  auto &registry = mAssetManager.getAssetRegistry();

  auto &asset = registry.getPrefabs().getAsset(
      static_cast<liquid::PrefabAssetHandle>(handle));
  auto parent = createEmptyEntity(camera, root, asset.name, saveToFile);

  uint32_t childIndex = 1;
  std::unordered_map<uint32_t, liquid::Entity> entityMap;

  auto getOrCreateEntity =
      [&entityMap, this, parent, &camera,
       &childIndex](uint32_t localId,
                    const liquid::LocalTransform &transform = {}) mutable {
        if (entityMap.find(localId) == entityMap.end()) {
          auto entity = createEmptyEntity(
              parent, transform, "Untitled " + std::to_string(childIndex));
          entityMap.insert_or_assign(localId, entity);
          childIndex++;
        }

        return entityMap.at(localId);
      };

  for (auto &item : asset.data.transforms) {
    liquid::LocalTransform transform{};
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

liquid::Entity EntityManager::getStartingCamera() {
  return getActiveScene().activeCamera;
}

liquid::Entity EntityManager::getActiveSimulationCamera() {
  return mSimulationScene.activeCamera;
}

void EntityManager::setStartingCamera(liquid::Entity camera) {
  getActiveScene().activeCamera = camera;
  mSceneIO.saveStartingCamera(camera, mScenePath / "main.lqscene");
}

liquid::LocalTransform
EntityManager::getTransformFromCamera(EditorCamera &camera) const {
  auto &scene = mInSimulation ? mSimulationScene : mScene;
  const auto &viewMatrix =
      scene.entityDatabase.get<liquid::Camera>(camera.getCamera()).viewMatrix;

  static constexpr glm::vec3 DistanceFromEye{0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(DistanceFromEye);

  liquid::LocalTransform transform;
  transform.localPosition = orientation[3];
  return transform;
}

} // namespace liquidator

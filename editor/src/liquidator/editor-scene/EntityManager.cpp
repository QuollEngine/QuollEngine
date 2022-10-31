#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"

#include "EntityManager.h"

namespace liquidator {

EntityManager::EntityManager(liquid::AssetManager &assetManager,
                             liquid::Renderer &renderer,
                             const std::filesystem::path &scenePath)
    : mScenePath(scenePath), mRenderer(renderer), mAssetManager(assetManager),
      mSceneIO(mAssetManager.getRegistry(), mScene) {}

void EntityManager::save(liquid::Entity entity) {
  if (mInSimulation)
    return;

  mSceneIO.saveEntity(entity, mScenePath / "main.lqscene");
}

liquid::Entity EntityManager::createEmptyEntity(
    liquid::Entity parent, const liquid::LocalTransformComponent &transform,
    const liquid::String &name) {

  auto &entityDatabase = getActiveEntityDatabase();

  auto entity = entityDatabase.create();
  entityDatabase.set(entity, transform);
  entityDatabase.set<liquid::WorldTransformComponent>(entity, {});

  if (entityDatabase.exists(parent)) {
    entityDatabase.set<liquid::ParentComponent>(entity, {parent});

    if (!entityDatabase.has<liquid::ChildrenComponent>(parent)) {
      entityDatabase.set<liquid::ChildrenComponent>(parent, {});
    }

    entityDatabase.get<liquid::ChildrenComponent>(parent).children.push_back(
        entity);
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
      mAssetManager.getRegistry().getSkeletons().getAsset(handle).data;

  liquid::SkeletonComponent skeletonInstance{};
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
  if (!entityDatabase.has<liquid::SkeletonComponent>(entity)) {
    return;
  }

  if (entityDatabase.has<liquid::SkeletonDebugComponent>(entity)) {
    entityDatabase.remove<liquid::SkeletonDebugComponent>(entity);
    return;
  }

  auto &skeleton = entityDatabase.get<liquid::SkeletonComponent>(entity);

  liquid::SkeletonDebugComponent skeletonDebug{};
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
  if (getActiveEntityDatabase().has<liquid::SkinnedMeshComponent>(entity)) {
    getActiveEntityDatabase().remove<liquid::SkinnedMeshComponent>(entity);
  }

  getActiveEntityDatabase().set<liquid::MeshComponent>(entity, {handle});
}

void EntityManager::setSkinnedMesh(liquid::Entity entity,
                                   liquid::SkinnedMeshAssetHandle handle) {
  if (getActiveEntityDatabase().has<liquid::MeshComponent>(entity)) {
    getActiveEntityDatabase().remove<liquid::MeshComponent>(entity);
  }
  getActiveEntityDatabase().set<liquid::SkinnedMeshComponent>(entity, {handle});
}

void EntityManager::setName(liquid::Entity entity, const liquid::String &name) {
  if (name.empty()) {
    return;
  }

  getActiveEntityDatabase().set<liquid::NameComponent>(entity, {name});
}

void EntityManager::setCamera(liquid::Entity entity,
                              const liquid::PerspectiveLensComponent &lens,
                              bool autoRatio) {
  getActiveEntityDatabase().set<liquid::CameraComponent>(entity, {});
  getActiveEntityDatabase().set<liquid::PerspectiveLensComponent>(entity, lens);
  if (autoRatio) {
    getActiveEntityDatabase().set<liquid::AutoAspectRatioComponent>(entity, {});
  }
}

void EntityManager::setAudio(liquid::Entity entity,
                             liquid::AudioAssetHandle source) {
  getActiveEntityDatabase().set<liquid::AudioSourceComponent>(entity, {source});
}

void EntityManager::setText(liquid::Entity entity, liquid::TextComponent text) {
  getActiveEntityDatabase().set(entity, text);
}

void EntityManager::setScript(liquid::Entity entity,
                              liquid::LuaScriptAssetHandle handle) {
  liquid::ScriptingComponent script{};
  script.handle = handle;
  getActiveEntityDatabase().set(entity, script);
}

void EntityManager::deleteEntity(liquid::Entity entity) {
  if (!mInSimulation) {
    mSceneIO.deleteEntityFilesAndRelations(entity, mScenePath / "main.lqscene");
  }

  getActiveEntityDatabase().set<liquid::DeleteComponent>(entity, {});
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

  auto &transform = entityDatabase.get<liquid::LocalTransformComponent>(entity);

  if (entityDatabase.has<liquid::ParentComponent>(entity)) {
    const auto parent =
        entityDatabase.get<liquid::ParentComponent>(entity).parent;
    if (entityDatabase.exists(parent) &&
        entityDatabase.has<liquid::WorldTransformComponent>(parent)) {
      const auto &parentWorld =
          entityDatabase.get<liquid::WorldTransformComponent>(parent)
              .worldTransform;

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

  auto &asset = mAssetManager.getRegistry().getPrefabs().getAsset(
      static_cast<liquid::PrefabAssetHandle>(handle));
  auto parent = createEmptyEntity(camera, root, asset.name, saveToFile);

  uint32_t childIndex = 1;
  std::unordered_map<uint32_t, liquid::Entity> entityMap;

  auto getOrCreateEntity =
      [&entityMap, this, parent, &camera, &childIndex](
          uint32_t localId,
          const liquid::LocalTransformComponent &transform = {}) mutable {
        if (entityMap.find(localId) == entityMap.end()) {
          auto entity = createEmptyEntity(
              parent, transform, "Untitled " + std::to_string(childIndex));
          entityMap.insert_or_assign(localId, entity);
          childIndex++;
        }

        return entityMap.at(localId);
      };

  for (auto &item : asset.data.transforms) {
    liquid::LocalTransformComponent transform{};
    transform.localPosition = item.value.position;
    transform.localRotation = item.value.rotation;
    transform.localScale = item.value.scale;

    getOrCreateEntity(item.entity, transform);
  }

  for (auto &item : asset.data.meshes) {
    if (!mAssetManager.getRegistry().getMeshes().hasAsset(item.value)) {
      continue;
    }

    setMesh(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.skinnedMeshes) {
    if (!mAssetManager.getRegistry().getSkinnedMeshes().hasAsset(item.value)) {
      continue;
    }

    setSkinnedMesh(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.skeletons) {
    if (!mAssetManager.getRegistry().getSkeletons().hasAsset(item.value)) {
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

liquid::LocalTransformComponent
EntityManager::getTransformFromCamera(EditorCamera &camera) const {
  auto &scene = mInSimulation ? mSimulationScene : mScene;
  const auto &viewMatrix =
      scene.entityDatabase.get<liquid::CameraComponent>(camera.getCamera())
          .viewMatrix;

  static constexpr glm::vec3 DistanceFromEye{0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(DistanceFromEye);

  liquid::LocalTransformComponent transform;
  transform.localPosition = orientation[3];
  return transform;
}

} // namespace liquidator
